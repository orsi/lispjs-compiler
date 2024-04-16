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

void parse_token(Token *token) {}

// 1 + 2 * 3
//
// should look like this at some point in the future :P
//      +
//     / \
//    1   *
//       / \
//      2   3
//
Node *parse(Token *token_start) {
  Token *current_token = token_start;
  int length = 2;
  int i = 0;
  Node *nodes = malloc(length * sizeof(Node));

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

      // push
      if (i < length) {
        printf("arr normal, i: %d length: %d\n", i, length);
        nodes[i] = *node;
      } else {
        length = length * 2;
        printf("arr expand,  i: %d length: %d\n", i, length);
        Node *new_nodes = malloc(length * sizeof(Node));
        memcpy(new_nodes, nodes, length);
        nodes[i] = *node;
      }

      current_token = current_token->next;
      i++;
      continue;
    }

    if (current_token->type == Symbol) {
      char symbol = current_token->value[0];
      printf("%c\n", symbol);
      node->type = BinaryExpression;
      node->symbol_value = symbol;

      // push
      if (i < length) {
        printf("arr normal, i: %d length: %d\n", i, length);
        nodes[i] = *node;
      } else {
        length = length * 2;
        printf("arr expand,  i: %d length: %d\n", i, length);
        Node *new_nodes = malloc(length * sizeof(Node));
        memcpy(new_nodes, nodes, length);
        nodes = new_nodes;
        nodes[i] = *node;
      }

      current_token = current_token->next;
      i++;
      continue;
    }

    // error!
    current_token = current_token->next;
  }

  return nodes;
}