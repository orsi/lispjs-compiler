#ifndef ROXANNE_H
#define ROXANNE_H

#include <stdlib.h>

// lexing
enum TokenType {
  TOKEN_IDENTIFIER,
  TOKEN_KEYWORD,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_SYMBOL,
};
typedef struct Token Token;
struct Token {
  enum TokenType type;
  int length;
  char *value;
  Token *next;
};

int is_keyword(char *word);
int string_starts_with(char *string, char *starts_with);
void print_tokens(Token *token);
Token *lex(char *input);

// parsing
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

// evaluate
enum ResultType { RESULT_NONE, RESULT_INTEGER, RESULT_DOUBLE };
typedef struct {
  enum ResultType type;
  union {
    double double_value;
    int int_value;
  };
} Result;

void print_result(Result *result);
Result *evaluate_binary_expression(Result *result, char symbol, Node *left_node,
                                   Node *right_node);
Result *evaluate_node(Node *node);

// utils
char *read_filepath(char *filepath);
#endif