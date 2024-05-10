#include "./lex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum NodeType { BinaryExpression, DoubleNumber, IntNumber, Root };
typedef struct Node Node;
struct Node {
  enum NodeType type;
  union {
    double double_value;
    int int_value;
    char symbol_value;
  };
  Node *left;
  Node *right;
};

Node *create_node() {
  Node *node = malloc(sizeof(Node));
  if (node == NULL) {
    printf("Error malloc node.");
    exit(1);
  }
  return node;
}

typedef struct {
  Node **items;
  size_t length;
  size_t capacity;
} NodeArray;

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
    printf("Pop 0 length array");
    exit(1);
  }

  node_array->length -= 1;
  return node_array->items[node_array->length];
}

void print_node_tree(Node *node, int level, char *prefix) {
  int indent = level * 2;
  if (node->type == Root) {
    printf("%*s└%sroot\n", indent, "", prefix);
  }
  if (node->type == BinaryExpression) {
    printf("%*s└%sbin: %c\n", indent, "", prefix, node->symbol_value);
  }
  if (node->type == DoubleNumber) {
    printf("%*s└%sdbl: %f\n", indent, "", prefix, node->double_value);
  }
  if (node->type == IntNumber) {
    printf("%*s└%sint: %d\n", indent, "", prefix, node->int_value);
  }

  if (node->left != NULL) {
    print_node_tree(node->left, level + 1, " l: ");
  }

  if (node->right != NULL) {
    print_node_tree(node->right, level + 1, " r: ");
  }
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
NodeArray *parse(Token *token_start, NodeArray *nodes) {
  Token *current_token = token_start;
  int length = 2;
  int i = 0;

  array_create(nodes, 50);

  while (current_token) {
    Node *node = malloc(sizeof(Node));

    // numbers
    if (current_token->type == Number) {
      if (strchr(current_token->value, '.')) {
        // double
        printf("double: %.*s\n", current_token->length, current_token->value);
        node->type = DoubleNumber;
        node->double_value = atof(current_token->value);
      } else {
        // int
        printf("int: %.*s\n", current_token->length, current_token->value);
        node->type = IntNumber;
        node->int_value = atoi(current_token->value);
      }

      array_push(nodes, node);
      current_token = current_token->next;
      i++;
      continue;
    }

    if (current_token->type == Symbol) {
      char symbol = current_token->value[0];
      printf("%c\n", symbol);
      node->type = BinaryExpression;
      node->symbol_value = symbol;

      array_push(nodes, node);

      current_token = current_token->next;
      i++;
      continue;
    }

    // error!
    current_token = current_token->next;
  }

  return nodes;
}