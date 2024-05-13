#ifndef PARSE_H
#define PARSE_H

#include "./lex.h"
#include <stdlib.h>

enum NodeType { NODE_EMPTY, NODE_BINARY_EXPRESSION, NODE_DOUBLE, NODE_INTEGER };
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
  Node **items;
  size_t length;
  size_t capacity;
} NodeArray;

Node *create_node();
NodeArray *create_node_array(NodeArray *node_array);
NodeArray *push_node_array(NodeArray *node_array, Node *item);
Node *pop_node_array(NodeArray *node_array);
void print_node_tree(Node *node, int level, char *prefix);
int get_operator_precedence(char s);
Node *parse_node(Token **tokens, NodeArray *node_stack);
Node *parse(Token *tokens);
#endif