#include "./roxanne.h"
#include <stdio.h>
#include <string.h>

char *get_node_string(Node *node) {
  char string[128];
  int length;
  switch (node->type) {
  case NODE_EMPTY: {
    length = sprintf(string, "empty");
    break;
  }
  case (NODE_EXPRESSION_BINARY): {
    length = sprintf(string, "bin: %c", node->expression_symbol);
    break;
  }
  case (NODE_VALUE_VARIABLE): {
    length = sprintf(string, "var: %.*s", node->variable_name.length,
                     node->variable_name.value);
    break;
  }
  case (NODE_VALUE_DOUBLE): {
    length = sprintf(string, "dbl: %f", node->value_double);
    break;
  }
  case (NODE_VALUE_INTEGER): {
    length = sprintf(string, "int: %d", node->value_int);
    break;
  }
  case (NODE_VALUE_STRING): {
    length = sprintf(string, "str: %.*s", node->value_string.length,
                     node->value_string.value);
    break;
  }
  }

  char *node_string = malloc(length);
  memcpy(node_string, string, length);
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

Node *create_node() {
  Node *node = malloc(sizeof(Node));
  if (node == NULL) {
    printf("Error malloc node.");
    exit(1);
  }
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
    printf("Error: Cannot pop 0 length array");
    exit(1);
  }

  node_array->length -= 1;
  return node_array->items[node_array->length];
}

// 1 + 2 * 3
//
// should look like this at some point in the future :P
//      +
//     / \
//    1   *
//       / \
//      2   3
//

int get_operator_precedence(char s) {
  if (s == '*' || s == '/' || s == '%') {
    return 1;
  } else {
    return 0;
  }
}

Node *parse_node(Token **tokens, NodeArray *node_stack) {
  Token current_token = **tokens;
  Node *node = create_node();

  if (current_token.type == TOKEN_NUMBER && strchr(current_token.value, '.')) {
    // double
    node->type = NODE_VALUE_DOUBLE;
    node->value_double = atof(current_token.value);
    *tokens = current_token.next;
    return node;
  }

  if (current_token.type == TOKEN_NUMBER) {
    // int
    node->type = NODE_VALUE_INTEGER;
    node->value_int = atoi(current_token.value);
    *tokens = current_token.next;
    return node;
  }

  if (current_token.type == TOKEN_STRING) {
    node->type = NODE_VALUE_STRING;
    node->value_string = (SimpleString){.length = current_token.length,
                                            .value = current_token.value};
    *tokens = current_token.next;
    return node;
  }

  if (current_token.type == TOKEN_IDENTIFIER) {
    node->type = NODE_VALUE_VARIABLE;
    node->variable_name = (SimpleString){.length = current_token.length,
                                             .value = current_token.value};
    *tokens = current_token.next;
    return node;
  }

  if (current_token.type == TOKEN_SYMBOL) {
    char symbol = current_token.value[0];

    if (symbol == '(') {
      *tokens = current_token.next;
      Node **root = &node;
      while (*tokens && (**tokens).value[0] != ')') {
        node = parse_node(tokens, node_stack);
        push_node_array(node_stack, node);
        *root = node;
      }
      *tokens = (**tokens).next;
      return *root;
    }

    Node *last_node = pop_node_array(node_stack);
    if (last_node != NULL && last_node->type == NODE_EXPRESSION_BINARY &&
        get_operator_precedence(last_node->expression_symbol) <
            get_operator_precedence(symbol)) {
      // when we have multiple binary expressions in a row, we
      // need to compare their operator precedence, and if the current
      // binary expression has greater precendence, we have to steal
      // the right operand from the lesser precedence, and replace
      // it with this binary expression
      node->type = NODE_EXPRESSION_BINARY;
      node->expression_symbol = symbol;
      node->left = last_node->right;
      last_node->right = node;
      *tokens = current_token.next;
      node->right = parse_node(tokens, node_stack);
      return last_node;
    } else {
      node->type = NODE_EXPRESSION_BINARY;
      node->expression_symbol = symbol;
      node->left = last_node;
      *tokens = current_token.next;
      node->right = parse_node(tokens, node_stack);
      return node;
    }
  }

  printf("Error: Could not parse token");
  exit(1);
}

Node *parse(Token *tokens) {
  Node *current_node = create_node();
  Node **root = &current_node;
  NodeArray stack;
  create_node_array(&stack);

  while (tokens) {
    Node *node = parse_node(&tokens, &stack);
    push_node_array(&stack, node);
    *root = node;
  }

  return *root;
}
