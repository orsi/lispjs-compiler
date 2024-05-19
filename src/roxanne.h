#ifndef ROXANNE_H
#define ROXANNE_H

#include <stdlib.h>

typedef struct {
  int length;
  char *value;
} SimpleString;

// lexing
enum TokenType {
  TOKEN_IDENTIFIER,
  TOKEN_KEYWORD,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_SYMBOL,
  TOKEN_END_OF_FILE,
};
typedef struct Token Token;
struct Token {
  enum TokenType type;
  char *value;
  int length;
  int line;
  int column;
  Token *next;
};

char *get_token_string(Token *token);
int is_keyword(char *word, int length);
void print_tokens(Token *token);
Token *lex(char *input);

// parsing
typedef struct {
  char *name;

} Variable;

enum NodeType {
  NODE_EXPRESSION_ASSIGNMENT,
  NODE_EXPRESSION_BINARY,
  NODE_STATEMENT_BLOCK,
  NODE_STATEMENT_CONDITIONAL,
  NODE_VALUE_DOUBLE,
  NODE_VALUE_INTEGER,
  NODE_VALUE_STRING,
  NODE_VALUE_VARIABLE,
};
typedef struct Node Node;
struct Node {
  enum NodeType type;
  union {
    char expression_symbol;
    double value_double;
    int value_int;
    SimpleString value_string;
    SimpleString value_variable;
    Node *conditional;
    Node *block;
  };
  Node *left;
  Node *right;
};

typedef struct {
  Node **items;
  size_t length;
  size_t capacity;
} NodeArray;

typedef struct {
  NodeArray *statements;
} Program;

char *get_node_string(Node *node);
void print_node_tree(Node *node, int level, char *prefix);
Node *create_node(enum NodeType type, void *value, Node *left, Node *right);
NodeArray *create_node_array(NodeArray *node_array);
NodeArray *push_node_array(NodeArray *node_array, Node *item);
Node *pop_node_array(NodeArray *node_array);
int get_operator_precedence(char s);
Node *parse_node(Token **tokens, NodeArray *node_stack);
Program *parse(Token *tokens);

// evaluate
enum ResultType { RESULT_NONE, RESULT_INTEGER, RESULT_DOUBLE, RESULT_STRING };
typedef struct {
  enum ResultType type;
  union {
    double value_double;
    int value_int;
    SimpleString value_string;
  };
} Result;

char *get_result_string(Result *result);
void print_result(Result *result);
Result *evaluate_binary_expression(Result *result, char symbol, Node *left_node,
                                   Node *right_node);
Result *evaluate(Node *node);

// utils
int str_starts_with(char *string, char *starts_with);
char *read_filepath(char *filepath);

#endif