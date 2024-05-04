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

typedef struct {
  Node *items;
  size_t length;
  size_t capacity;
} NodeArray;

NodeArray *array_create(NodeArray *nodes, size_t initial_capacity) {
  nodes->items = malloc(initial_capacity * sizeof(Node));
  if (nodes->items == NULL) {
    printf("Error malloc node array.");
    exit(1);
  }
  nodes->length = 0;
  nodes->capacity = initial_capacity;
  return nodes;
}

NodeArray *array_push(NodeArray *nodes, Node *item) {
  if (nodes->length >= nodes->capacity) {
    nodes->capacity *= 2;
    nodes->items = realloc(nodes->items, nodes->capacity * sizeof(Node));
    if (nodes->items == NULL) {
      printf("Error realloc node array.");
      exit(1);
    }
  }
  nodes->items[nodes->length] = *item;
  nodes->length += 1;
  return nodes;
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