#include "roxanne.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *keywords[] = {"if",    "import", "else", "export",
                                 "false", "return", "true"};

int is_keyword(char *word, int length) {
  int is_match = 0;
  for (size_t i = 0; i < sizeof(keywords) / sizeof(*keywords); i++) {
    const char *key = keywords[i];
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

int get_operator_precedence(char *s) {
  if (starts_with(s, "*") || starts_with(s, "/") || starts_with(s, "%%")) {
    return 2;
  } else if (starts_with(s, "+") || starts_with(s, "-")) {
    return 1;
  } else {
    return 0;
  }
}

Node *create_node(enum NodeType type, void *value) {
  Node *node = malloc(sizeof(Node));
  if (node == NULL) {
    printf("Error malloc node.\n");
    exit(1);
  }

  node->type = type;
  switch (node->type) {
  case NODE_EXPRESSION:
  case NODE_STATEMENT_BLOCK:
  case NODE_STATEMENT_CONDITIONAL: {
    NodeConditional *conditional = value;
    node->expression = conditional->expression;
    node->if_then = conditional->if_then;
    node->if_else = conditional->if_else;
    break;
  }
  case NODE_EXPRESSION_ASSIGNMENT:
  case NODE_EXPRESSION_BINARY: {
    NodeBinary *expression = value;
    node->operator_symbol = expression->operator_symbol;
    node->left = expression->left;
    node->right = expression->right;
    break;
  }
  case NODE_LITERAL_ARRAY:
    node->array = value;
    break;
  case NODE_LITERAL_BOOLEAN:
    node->boolean = *(bool *)value;
    break;
  case NODE_LITERAL_FUNCTION:
    node->function = value;
    break;
  case NODE_LITERAL_IDENTIFIER:
    node->identifier = value;
    break;
  case NODE_LITERAL_STRING:
    node->string = value;
    break;
  case NODE_LITERAL_STRING_TEMPLATE:
    node->string_template_parts = value;
    break;
  case NODE_LITERAL_NUMBER:
    node->number = *(double *)value;
    break;
  case NODE_LITERAL_OBJECT:
    node->object = value;
    break;
  case NODE_PROGRAM:
    node->body = (Node *)value;
    break;
  case NODE_STATEMENT_MULTI:
    node->statements = value;
    break;
  }

  return node;
}

Node *parse_program(Token *token) {
  Node head = {0};
  Node *current = &head;
  Node *root = NULL;

  while (token) {
    // end file
    if (token->type == TOKEN_END_OF_FILE) {
      if (root != NULL) {
        current = current->next = root;
      }
      break;
    }

    // statement terminator
    if (token->type == TOKEN_SYMBOL && starts_with(token->value, ";")) {
      current = current->next = root;
      root = NULL;
      token = token->next;
      continue;
    }

    root = parse(&token, root);
  }

  Node *node_program = create_node(NODE_PROGRAM, head.next);
  return node_program;
}

Node *parse(Token **tokens, Node *last_node) {
  Token *current_token = *tokens;

  // literal number
  if (current_token->type == TOKEN_NUMBER) {
    // strip all spaces
    char normalizedNumber[current_token->length + 1];
    char *position = current_token->value;
    size_t i = 0;
    while ((size_t)(position - current_token->value) < current_token->length) {
      if (position[0] != ' ') {
        normalizedNumber[i] = position[0];
        i++;
      }
      position++;
    }
    normalizedNumber[i] = '\0';
    double value = atof(normalizedNumber);
    Node *node = create_node(NODE_LITERAL_NUMBER, &value);
    *tokens = current_token->next;
    return node;
  }

  // literal binary number
  if (current_token->type == TOKEN_NUMBER_ALTERNATIVE_BASE &&
      starts_with(current_token->value, "#")) {
    // strip all spaces from value and provide null terminator
    char normalizedNumber[current_token->length + 1];
    char *position = current_token->value;
    position++; // # skip
    int base = position[0] == 'b' ? 2 : position[0] == 'o' ? 8 : 16;
    position++; // b/h/o skip
    size_t i = 0;
    while ((size_t)(position - current_token->value) < current_token->length) {
      if (position[0] != ' ') {
        normalizedNumber[i] = position[0];
        i++;
      }
      position++;
    }
    normalizedNumber[i] = '\0';
    double value = strtol(normalizedNumber, NULL, base);
    Node *node = create_node(NODE_LITERAL_NUMBER, &value);
    *tokens = current_token->next;
    return node;
  }

  // literal string
  if (current_token->type == TOKEN_STRING) {
    String *value = malloc(sizeof(String));
    if (value == NULL) {
      printf("Error: cannot malloc value\n");
      exit(1);
    }
    value->length = current_token->length;
    value->value = current_token->value;
    Node *node = create_node(NODE_LITERAL_STRING, value);
    *tokens = current_token->next;
    return node;
  }

  // literal string interpolation
  if (current_token->type == TOKEN_STRING_TEMPLATE_START) {
    Array *node_template_parts = create_array();

    current_token = current_token->next; // skip template start token
    while (current_token && current_token->type != TOKEN_STRING_TEMPLATE_END) {
      if (current_token->type == TOKEN_STRING_TEMPLATE_PART_START) {
        current_token = current_token->next; // skip template part start token
        Node *current_root = NULL;
        while (current_token &&
               current_token->type != TOKEN_STRING_TEMPLATE_PART_END) {
          current_root = parse(&current_token, current_root);
        }
        push_array(node_template_parts, current_root);
        current_token = current_token->next; // skip template part end token
      } else {
        Node *node_literal_string = parse(&current_token, NULL);
        push_array(node_template_parts, node_literal_string);
      }
    }

    Node *node = create_node(NODE_LITERAL_STRING_TEMPLATE, node_template_parts);
    *tokens = current_token->next;
    return node;
  }

  // literal array
  if (current_token->type == TOKEN_SYMBOL &&
      starts_with(current_token->value, "[")) {
    current_token = current_token->next;
    Node *root = NULL;
    Array *node_literals = create_array();
    while (current_token) {
      if (starts_with(current_token->value, "]")) {
        if (root != NULL) {
          push_array(node_literals, root);
          root = NULL;
        }
        break;
      }

      if (current_token->type == TOKEN_SYMBOL &&
          (starts_with(current_token->value, ";") ||
           starts_with(current_token->value, ","))) {
        current_token = current_token->next;
        push_array(node_literals, root);
        root = NULL;
        continue;
      }

      Node *node = parse(&current_token, root);
      root = node;
    }

    Node *node = create_node(NODE_LITERAL_ARRAY, node_literals);
    *tokens = current_token->next;
    return node;
  }

  // literal object
  if (current_token->type == TOKEN_SYMBOL &&
      starts_with(current_token->value, "{")) {
    current_token = current_token->next;
    Node *root = NULL;
    Array *node_assignments = create_array();
    while (current_token) {
      if (starts_with(current_token->value, "}")) {
        if (root != NULL) {
          push_array(node_assignments, root);
          root = NULL;
        }
        break;
      }

      if (current_token->type == TOKEN_SYMBOL &&
          starts_with(current_token->value, ";")) {
        // end of current statement
        current_token = current_token->next;
        push_array(node_assignments, root);
        root = NULL;
        continue;
      }

      Node *node = parse(&current_token, root);
      root = node;
    }

    Node *node = create_node(NODE_LITERAL_OBJECT, node_assignments);
    *tokens = current_token->next;
    return node;
  }

  // identifier literal: true
  if (current_token->type == TOKEN_IDENTIFIER &&
      starts_with(current_token->value, "true")) {
    bool value = true;
    Node *node = create_node(NODE_LITERAL_BOOLEAN, &value);
    *tokens = current_token->next;
    return node;
  }

  // identifier literal: false
  if (current_token->type == TOKEN_IDENTIFIER &&
      starts_with(current_token->value, "false")) {
    bool value = false;
    Node *node = create_node(NODE_LITERAL_BOOLEAN, &value);
    *tokens = current_token->next;
    return node;
  }

  // identifier keyword: if
  if (current_token->type == TOKEN_IDENTIFIER &&
      starts_with(current_token->value, "if")) {
    *tokens = current_token->next;
    Node *condition_root = NULL;
    while (*tokens && (*tokens)->value[0] != '{') {
      Node *condition_node = parse(tokens, NULL);
      condition_root = condition_node;
    }
    NodeConditional value = {condition_root, parse(tokens, last_node),
                             parse(tokens, last_node)};
    Node *node = create_node(NODE_STATEMENT_CONDITIONAL, &value);
    return node;
  }

  // identifier keyword: else
  // the else keyword is only relevant as a 'block' statement the right side
  // of an if conditional, so we skip to the nearest opening block
  if (current_token->type == TOKEN_IDENTIFIER &&
      starts_with(current_token->value, "else")) {
    *tokens = current_token->next;
    while (*tokens && (*tokens)->value[0] != '{') {
      *tokens = (*tokens)->next;
    }
    return parse(tokens, last_node);
  }

  // identifier
  if (current_token->type == TOKEN_IDENTIFIER) {
    size_t size = current_token->length + 1;
    char *value = malloc(sizeof(char *) * size);
    if (value == NULL) {
      printf("Error: cannot malloc value\n");
      exit(1);
    }
    value = strncpy(value, current_token->value, current_token->length);
    value[size] = '\0';
    Node *node = create_node(NODE_LITERAL_IDENTIFIER, value);
    *tokens = current_token->next;
    return node;
  }

  // expression sub-tree
  if (current_token->type == TOKEN_SYMBOL &&
      starts_with(current_token->value, "(")) {
    *tokens = current_token->next;
    Node *subtree_root = NULL;
    while (*tokens) {
      if ((*tokens)->value[0] == ')') {
        *tokens = (*tokens)->next; // skip last )
        break;
      }
      Node *subtree_node = parse(tokens, subtree_root);
      subtree_root = subtree_node;
    }

    Node *node = create_node(NODE_EXPRESSION, subtree_root);
    return node;
  }

  char *operator_symbol = get_operator(current_token);

  // assignment expression
  if (current_token->type == TOKEN_SYMBOL &&
      starts_with(operator_symbol, ":")) {
    *tokens = current_token->next;
    NodeBinary value = {.operator_symbol = operator_symbol,
                        .left = last_node,
                        .right = parse(tokens, NULL)};
    Node *node = create_node(NODE_EXPRESSION_ASSIGNMENT, &value);
    return node;
  }

  // elision
  if (current_token->type == TOKEN_SYMBOL &&
      starts_with(current_token->value, ",")) {

    Array *statements = create_array();
    push_array(statements, last_node);

    *tokens = current_token->next;
    Node *root = NULL;
    while (*tokens) {
      current_token = *tokens;
      if (current_token->type == TOKEN_SYMBOL &&
          (starts_with((*tokens)->value, ";") ||
           starts_with((*tokens)->value, ")"))) {
        push_array(statements, root);
        break;
      }

      if (current_token->type == TOKEN_SYMBOL &&
          starts_with(current_token->value, ",")) {
        push_array(statements, root);
        continue;
      }

      root = parse(tokens, root);
    }

    Node *node = create_node(NODE_STATEMENT_MULTI, statements);
    return node;
  }

  // function
  if (current_token->type == TOKEN_SYMBOL &&
      starts_with(current_token->value, "=>") &&
      last_node->type == NODE_EXPRESSION) {
    *tokens = (*tokens)->next; // skip =
    *tokens = (*tokens)->next; // skip >

    Node *block = parse(tokens, NULL);
    Function *function = malloc(sizeof(*function));
    function->parameters = last_node;
    function->block = block;
    Node *node = create_node(NODE_LITERAL_FUNCTION, function);
    return node;
  }

  // binary expression
  if (current_token->type == TOKEN_SYMBOL &&
      (starts_with(current_token->value, "+") ||
       starts_with(current_token->value, "-") ||
       starts_with(current_token->value, "/") ||
       starts_with(current_token->value, "*") ||
       starts_with(current_token->value, "%%") ||
       starts_with(current_token->value, "<") ||
       starts_with(current_token->value, ">") ||
       starts_with(current_token->value, "<=") ||
       starts_with(current_token->value, ">="))) {
    if (last_node != NULL && last_node->type == NODE_EXPRESSION_BINARY &&
        get_operator_precedence(last_node->operator_symbol) <
            get_operator_precedence(current_token->value)) {
      // when we have multiple binary expressions in a row, we
      // need to compare their operator precedence, and if the current
      // binary expression has greater precendence, we have to steal
      // the right operand from the lesser precedence, and replace
      // it with this binary expression
      *tokens = current_token->next;
      NodeBinary value = {.operator_symbol = operator_symbol,
                          .left = last_node->right,
                          .right = parse(tokens, NULL)};
      Node *node = create_node(NODE_EXPRESSION_BINARY, &value);
      last_node->right = node;
      return last_node;
    } else {
      *tokens = current_token->next;
      NodeBinary value = {.operator_symbol = operator_symbol,
                          .left = last_node,
                          .right = parse(tokens, NULL)};
      Node *node = create_node(NODE_EXPRESSION_BINARY, &value);
      return node;
    }
  }

  printf("Error: Could not parse %s\n", stringify_token(current_token));
  exit(1);
}
