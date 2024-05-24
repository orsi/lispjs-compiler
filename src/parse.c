#include "./roxanne.h"
#include <ctype.h>
#include <malloc/_malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *keywords[] = {"if",    "import", "else", "export",
                           "false", "return", "true"};

int is_keyword(char *word, int length) {
  int is_match = 0;
  for (int i = 0; i < sizeof(keywords) / sizeof(*keywords); i++) {
    char *key = keywords[i];
    is_match = strncmp(word, key, length) == 0; // i hate this
    if (is_match)
      break;
  }
  return is_match;
}

char *get_operator(Token *tokens) {
  Token *current_token = tokens;
  size_t length = 0;

  while (current_token && length < current_token->length &&
         ispunct(current_token->value[0])) {
    length += 1;
    current_token = tokens->next;
  }

  char *operator_symbol = malloc(sizeof(char) * length);
  strncpy(operator_symbol, tokens->value, length);
  return operator_symbol;
}

bool is_symbol(Token *tokens, char *operator_symbol) {
  size_t length = strlen(operator_symbol);
  Token *current_token = tokens;

  for (size_t i = 0; i < length; i++) {
    if (current_token->value[0] != operator_symbol[i]) {
      return false;
    }
    current_token = tokens->next;
  }

  return true;
}

int get_operator_precedence(char *s) {
  if (exact(s, "*") || exact(s, "/") || exact(s, "%%")) {
    return 2;
  } else if (exact(s, "+") || exact(s, "-")) {
    return 1;
  } else {
    return 0;
  }
}

Node *create_node(enum NodeType type, void *value, Node *left, Node *right) {
  Node *node = malloc(sizeof(Node));
  if (node == NULL) {
    printf("Error malloc node.");
    exit(1);
  }

  node->type = type;
  switch (node->type) {
  case NODE_EXPRESSION:
  case NODE_STATEMENT_BLOCK:
  case NODE_STATEMENT_CONDITIONAL:
    node->expression = (Node *)value;
    break;
  case NODE_EXPRESSION_ASSIGNMENT:
  case NODE_EXPRESSION_BINARY:
    node->operator_symbol = (char *)value;
    break;
  case NODE_LITERAL_ARRAY:
    node->array = *(Array *)value;
    break;
  case NODE_LITERAL_BOOLEAN:
    node->boolean = *(bool *)value;
    break;
  case NODE_LITERAL_IDENTIFIER:
    node->identifier = (char *)value;
    break;
  case NODE_LITERAL_STRING:
    node->string = *(String *)value;
    break;
  case NODE_LITERAL_NUMBER:
    node->number = *(double *)value;
    break;
  case NODE_LITERAL_OBJECT:
    node->object = *(Object *)value;
    break;
  }

  node->left = left;
  node->right = right;
  return node;
}

Node *parse_expression(Token **tokens, Node *last_node) {
  Token *current_token = *tokens;

  // number literal
  if (current_token->type == TOKEN_NUMBER) {
    // strip all spaces
    char normalizedNumber[current_token->length + 1];
    char *position = current_token->value;
    size_t i = 0;
    while (position - current_token->value < current_token->length) {
      if (position[0] != ' ') {
        normalizedNumber[i] = position[0];
        i++;
      }
      position++;
    }
    normalizedNumber[i] = '\0';
    double value = atof(normalizedNumber);
    Node *node = create_node(NODE_LITERAL_NUMBER, &value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  // string literal
  if (current_token->type == TOKEN_STRING) {
    String *value = malloc(sizeof(String));
    if (value == NULL) {
      printf("Error: cannot malloc value");
      exit(1);
    }
    value->length = current_token->length;
    value->value = current_token->value;
    Node *node = create_node(NODE_LITERAL_STRING, value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  // identifier: true
  if (current_token->type == TOKEN_IDENTIFIER &&
      starts_with(current_token->value, "true")) {
    bool value = true;
    Node *node = create_node(NODE_LITERAL_BOOLEAN, &value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  // identifier: false
  if (current_token->type == TOKEN_IDENTIFIER &&
      starts_with(current_token->value, "false")) {
    bool value = false;
    Node *node = create_node(NODE_LITERAL_BOOLEAN, &value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  // keyword: if
  if (current_token->type == TOKEN_IDENTIFIER &&
      starts_with(current_token->value, "if")) {
    *tokens = current_token->next;
    Node *condition_root = NULL;
    while (*tokens && (*tokens)->value[0] != '{') {
      Node *condition_node = parse_expression(tokens, NULL);
      condition_root = condition_node;
    }
    Node *node = create_node(NODE_STATEMENT_CONDITIONAL, condition_root,
                             parse_expression(tokens, last_node),
                             parse_expression(tokens, last_node));
    return node;
  }

  // keyword: else
  // the else keyword is only relevant as a 'block' statement the right side
  // of an if conditional, so we skip to the nearest opening block
  if (current_token->type == TOKEN_IDENTIFIER &&
      starts_with(current_token->value, "else")) {
    *tokens = current_token->next;
    while (*tokens && (*tokens)->value[0] != '{') {
      *tokens = (*tokens)->next;
    }
    return parse_expression(tokens, last_node);
  }

  // identifier
  if (current_token->type == TOKEN_IDENTIFIER) {
    size_t size = current_token->length + 1;
    char *value = malloc(sizeof(char *) * size);
    if (value == NULL) {
      printf("Error: cannot malloc value");
      exit(1);
    }
    value = strncpy(value, current_token->value, current_token->length);
    value[size] = '\0';
    Node *node = create_node(NODE_LITERAL_IDENTIFIER, value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  // expression sub-tree
  if (current_token->type == TOKEN_SYMBOL && is_symbol(current_token, "(")) {
    *tokens = current_token->next;
    Node *subtree_root = NULL;
    while (*tokens) {
      if ((*tokens)->value[0] == ')') {
        *tokens = (*tokens)->next; // skip last )
        break;
      }
      Node *subtree_node = parse_expression(tokens, last_node);
      subtree_root = subtree_node;
    }

    Node *node = create_node(NODE_EXPRESSION, subtree_root, NULL, NULL);
    return node;
  }

  // block statement
  if (current_token->type == TOKEN_SYMBOL && is_symbol(current_token, "{")) {
    *tokens = current_token->next;
    Node *block_root = NULL;
    while (*tokens) {
      if (starts_with((*tokens)->value, "}")) {
        *tokens = (*tokens)->next; // skip last }
        break;
      }
      Node *block_node = parse_expression(tokens, NULL);
      block_root = block_node;
    }

    Node *node = create_node(NODE_STATEMENT_BLOCK, block_root, NULL, NULL);
    return node;
  }

  char *operator_symbol = get_operator(current_token);

  // assignment expression
  if (current_token->type == TOKEN_SYMBOL && exact(operator_symbol, ":")) {
    *tokens = current_token->next;
    Node *node = create_node(NODE_EXPRESSION_ASSIGNMENT, operator_symbol,
                             last_node, parse_expression(tokens, last_node));
    return node;
  }

  // TODO: ?
  // unary expression
  // if (current_token->type == TOKEN_SYMBOL && last_node == NULL) {
  // }

  // binary expression
  if (current_token->type == TOKEN_SYMBOL) {
    if (last_node != NULL && last_node->type == NODE_EXPRESSION_BINARY &&
        get_operator_precedence(last_node->operator_symbol) <
            get_operator_precedence(current_token->value)) {
      // when we have multiple binary expressions in a row, we
      // need to compare their operator precedence, and if the current
      // binary expression has greater precendence, we have to steal
      // the right operand from the lesser precedence, and replace
      // it with this binary expression
      *tokens = current_token->next;
      Node *node = create_node(NODE_EXPRESSION_BINARY, operator_symbol,
                               last_node->right, NULL);
      node->right = parse_expression(tokens, NULL);
      last_node->right = node;
      return last_node;
    } else {
      *tokens = current_token->next;
      Node *node =
          create_node(NODE_EXPRESSION_BINARY, operator_symbol, last_node, NULL);
      node->right = parse_expression(tokens, NULL);
      return node;
    }
  }

  printf("Error: Could not parse %s", get_token_string(current_token));
  exit(1);
}

Program *parse(Token *tokens) {
  Program *program = malloc(sizeof(Program));
  if (program == NULL) {
    printf("Error: cannot malloc program.");
    exit(1);
  }
  program->statements = create_array();

  // root of current node tree
  Node *current_root = NULL;

  // parse statements until end of file
  while (tokens) {
    // statement terminator
    if (tokens->type == TOKEN_SYMBOL && starts_with(tokens->value, ";")) {
      tokens = tokens->next;
      push_array(program->statements, current_root);
      continue;
    }

    // end file
    if (tokens->type == TOKEN_END_OF_FILE) {
      push_array(program->statements, current_root);
      break;
    }

    current_root = parse_expression(&tokens, current_root);
  }

  return program;
}
