#include "./roxanne.h"
#include <malloc/_malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_node_string(Node *node) {
  char buffer[128];
  int length;

  if (node == NULL) {
    return NULL;
  }

  switch (node->type) {
  case (NODE_EXPRESSION_BINARY):
    length = sprintf(buffer, "node:binary, %c", node->expression_symbol);
    break;
  case (NODE_EXPRESSION_ASSIGNMENT):
    length = sprintf(buffer, "node:assignment, %c", node->expression_symbol);
    break;
  case (NODE_VALUE_VARIABLE):
    length = sprintf(buffer, "node:variable, %.*s", node->value_variable.length,
                     node->value_variable.value);
    break;
  case (NODE_VALUE_INTEGER):
    length = sprintf(buffer, "node:integer, %d", node->value_int);
    break;
  case (NODE_VALUE_DOUBLE):
    length = sprintf(buffer, "node:double, %f", node->value_double);
    break;
  case (NODE_VALUE_STRING):
    length = sprintf(buffer, "node:string, %.*s", node->value_string.length,
                     node->value_string.value);
    break;
  case NODE_STATEMENT_CONDITIONAL:
    length = sprintf(buffer, "node:conditional, %s",
                     get_node_string(node->conditional));
    break;
  case NODE_STATEMENT_BLOCK:
    length = sprintf(buffer, "node:block, %s", get_node_string(node->block));
    break;
  }

  char *node_string = malloc(length + 1);
  if (node_string == NULL) {
    printf("Error: cannot malloc node_string");
    exit(1);
  }
  strcpy(node_string, buffer);
  return node_string;
}

void print_node_tree(Node *node, int level, char *prefix) {
  int indent = level * 2;
  printf("%*s└ %s%s\n", indent, "", prefix, get_node_string(node));

  if (node->left != NULL) {
    print_node_tree(node->left, level + 1, "l: ");
  }

  if (node->right != NULL) {
    print_node_tree(node->right, level + 1, "r: ");
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
  case NODE_EXPRESSION_ASSIGNMENT:
    node->expression_symbol = *(char *)value;
    break;
  case NODE_EXPRESSION_BINARY:
    node->expression_symbol = *(char *)value;
    break;
  case NODE_STATEMENT_BLOCK:
    node->block = (Node *)value;
    break;
  case NODE_STATEMENT_CONDITIONAL:
    node->conditional = (Node *)value;
    break;
  case NODE_VALUE_DOUBLE:
    node->value_double = *(double *)value;
    break;
  case NODE_VALUE_INTEGER:
    node->value_int = *(int *)value;
    break;
  case NODE_VALUE_STRING:
    node->value_string = *(SimpleString *)value;
    break;
  case NODE_VALUE_VARIABLE:
    node->value_variable = *(SimpleString *)value;
    break;
  }

  node->left = left;
  node->right = right;
  return node;
}

NodeArray *create_node_array(NodeArray *node_array) {
  node_array->items = malloc(5 * sizeof(node_array->items));
  if (node_array->items == NULL) {
    printf("Could not malloc.");
    exit(1);
  }
  node_array->capacity = 5;
  node_array->length = 0;
  return node_array;
}

NodeArray *push_node_array(NodeArray *node_array, Node *item) {
  if (node_array->length == node_array->capacity) {
    node_array->capacity *= 2;
    node_array->items = realloc(node_array->items, node_array->capacity);
    if (node_array->items == NULL) {
      printf("Could not realloc.");
      exit(1);
    }
  }

  node_array->items[node_array->length] = item;
  node_array->length += 1;

  return node_array;
}

Node *pop_node_array(NodeArray *node_array) {
  if (node_array->length == 0) {
    return NULL;
  }

  node_array->length -= 1;
  return node_array->items[node_array->length];
}

int get_operator_precedence(char s) {
  switch (s) {
  case '*':
  case '/':
  case '%':
    return 2;
  case '+':
  case '-':
    return 1;
  case '<':
  // case '<=':
  case '>':
  // case '>=':
  default:
    return 0;
  }
}

Node *parse_node(Token **tokens, NodeArray *node_stack) {
  Token *current_token = *tokens;

  // quick return statement terminator
  if (current_token->type == TOKEN_SYMBOL && current_token->value[0] == ';') {
    *tokens = current_token->next;
    return NULL;
  }

  // double
  if (current_token->type == TOKEN_NUMBER &&
      memchr(current_token->value, '.', current_token->length) != NULL) {
    double value = atof(current_token->value);
    Node *node = create_node(NODE_VALUE_DOUBLE, &value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  if (current_token->type == TOKEN_NUMBER) {
    // int
    int value = atoi(current_token->value);
    Node *node = create_node(NODE_VALUE_INTEGER, &value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  if (current_token->type == TOKEN_STRING) {
    SimpleString *value = malloc(sizeof(SimpleString));
    if (value == NULL) {
      printf("Error: cannot malloc value");
      exit(1);
    }
    value->length = current_token->length;
    value->value = current_token->value;
    Node *node = create_node(NODE_VALUE_STRING, value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  // keywords
  if (current_token->type == TOKEN_KEYWORD) {
    if (str_starts_with(current_token->value, "if")) {
      *tokens = current_token->next;
      Node *condition_root = NULL;
      NodeArray condition_stack;
      create_node_array(&condition_stack);
      while (*tokens && (*tokens)->value[0] != '{') {
        Node *condition_node = parse_node(tokens, &condition_stack);
        push_node_array(&condition_stack, condition_node);
        condition_root = condition_node;
      }
      Node *node = create_node(NODE_STATEMENT_CONDITIONAL, condition_root,
                               parse_node(tokens, node_stack),
                               parse_node(tokens, node_stack));
      return node;
    }

    // the else keyword is only relevant as a 'block' statement the right side
    // of an if conditional, so we skip to the nearest opening block
    if (str_starts_with(current_token->value, "else")) {
      *tokens = current_token->next;
      while (*tokens && (*tokens)->value[0] != '{') {
        *tokens = (*tokens)->next;
      }
      return parse_node(tokens, node_stack);
    }
  }

  // identifiers
  if (current_token->type == TOKEN_IDENTIFIER) {
    SimpleString *value = malloc(sizeof(SimpleString));
    if (value == NULL) {
      printf("Error: cannot malloc value");
      exit(1);
    }
    value->length = current_token->length;
    value->value = current_token->value;
    Node *node = create_node(NODE_VALUE_VARIABLE, value, NULL, NULL);
    *tokens = current_token->next;
    return node;
  }

  Node *last_node = pop_node_array(node_stack);

  if (current_token->type == TOKEN_SYMBOL) {
    char symbol = current_token->value[0];

    // sub-trees
    if (symbol == '(') {
      *tokens = current_token->next;
      Node *subtree_root = NULL;
      while (*tokens) {
        if ((*tokens)->value[0] == ')') {
          *tokens = (*tokens)->next; // skip last )
          break;
        }
        Node *subtree_node = parse_node(tokens, node_stack);
        push_node_array(node_stack, subtree_node);
        subtree_root = subtree_node;
      }
      return subtree_root;
    }

    if (symbol == '{') {
      *tokens = current_token->next;
      Node *block_root = NULL;
      NodeArray block_stack;
      create_node_array(&block_stack);
      while (*tokens) {
        if ((*tokens)->value[0] == '}') {
          *tokens = (*tokens)->next; // skip last }
          break;
        }
        Node *block_node = parse_node(tokens, &block_stack);
        push_node_array(&block_stack, block_node);
        block_root = block_node;
      }

      Node *node = create_node(NODE_STATEMENT_BLOCK, block_root, NULL, NULL);
      return node;
    }

    // assignment expressions
    if (symbol == ':') {
      *tokens = current_token->next;
      Node *node = create_node(NODE_EXPRESSION_ASSIGNMENT, &symbol, last_node,
                               parse_node(tokens, node_stack));
      return node;
    }

    // binary expressions
    if (last_node != NULL && last_node->type == NODE_EXPRESSION_BINARY &&
        get_operator_precedence(last_node->expression_symbol) <
            get_operator_precedence(symbol)) {
      // when we have multiple binary expressions in a row, we
      // need to compare their operator precedence, and if the current
      // binary expression has greater precendence, we have to steal
      // the right operand from the lesser precedence, and replace
      // it with this binary expression
      *tokens = current_token->next;
      Node *node =
          create_node(NODE_EXPRESSION_BINARY, &symbol, last_node->right,
                      parse_node(tokens, node_stack));
      last_node->right = node;
      return last_node;
    } else {
      *tokens = current_token->next;
      Node *node = create_node(NODE_EXPRESSION_BINARY, &symbol, last_node,
                               parse_node(tokens, node_stack));
      return node;
    }
  }

  // eof
  if (current_token->type == TOKEN_END_OF_FILE) {
    *tokens = current_token->next;
    return last_node;
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
  program->statements = malloc(sizeof(NodeArray));
  create_node_array(program->statements);

  // we need a stack of nodes in order to piece together
  // binary expressions, assignment expressions, conditionals, etc.
  NodeArray current_node_stack;
  create_node_array(&current_node_stack);
  Node *current_root = NULL;
  while (tokens && tokens->type != TOKEN_END_OF_FILE) {
    Node *node = parse_node(&tokens, &current_node_stack);
    if (node == NULL) {
      // root node is tree for entire statement
      push_node_array(program->statements, current_root);
      // reset stack
      create_node_array(&current_node_stack);
      continue;
    }
    push_node_array(&current_node_stack, node);
    current_root = node;
  }

  // root node is tree for entire statement
  push_node_array(program->statements, current_root);
  return program;
}
