#ifndef ROXANNE_H
#define ROXANNE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  void **keys;
} Object;

typedef struct {
  size_t item_size;
  size_t length;
  size_t capacity;
  void **items;
} Array;

typedef struct {
  int length;
  char *value;
} String;

// lexing
enum TokenType {
  TOKEN_IDENTIFIER,
  TOKEN_NUMBER,
  TOKEN_NUMBER_ALTERNATIVE_BASE,
  TOKEN_STRING,
  TOKEN_SYMBOL,
  TOKEN_END_OF_FILE,
};
typedef struct Token Token;
struct Token {
  enum TokenType type;
  char *value;
  size_t length;
  int line;
  int column;
  Token *next;
};

Token *create_token(enum TokenType type, char *string, int length);
Token *lex(char *input);

// parsing
enum NodeType {
  NODE_EXPRESSION,
  NODE_EXPRESSION_ASSIGNMENT,
  NODE_EXPRESSION_BINARY,
  NODE_STATEMENT_BLOCK,
  NODE_STATEMENT_CONDITIONAL,
  NODE_LITERAL_ARRAY,
  NODE_LITERAL_BOOLEAN,
  NODE_LITERAL_IDENTIFIER,
  NODE_LITERAL_NUMBER,
  NODE_LITERAL_OBJECT,
  NODE_LITERAL_STRING,
};
typedef struct Node Node;
struct Node {
  enum NodeType type;
  Node *left;
  Node *right;
  union {
    bool boolean;
    char *operator_symbol;
    char *identifier;
    double number;
    String string;
    Node *expression;
    Array array;
    Object object;
  };
};
typedef struct {
  Array *statements;
} Program;

int is_keyword(char *word, int length);
bool is_symbol(Token *tokens, const char *operator_symbol);
char *get_operator(Token *tokens);
Node *create_node(enum NodeType type, void *value, Node *left, Node *right);
int get_operator_precedence(char *s);
Node *parse_expression(Token **tokens, Node *last_node);
Program *parse(Token *tokens);

// evaluate
enum ResultType { RESULT_NONE, RESULT_NUMBER, RESULT_STRING };
typedef struct {
  enum ResultType type;
  union {
    double number;
    String string;
  };
} Result;
Result *evaluate_assignment_expression(Result *result, Node *left_node,
                                       Node *right_node);
Result *evaluate_binary_expression(Result *result, char *operator_symbol,
                                   Node *left_node, Node *right_node);
Result *evaluate(Node *node);

// utils
bool starts_with(const char *a, const char *b);
char *read_filepath(const char *filepath);

Array *create_array(void);
Array *push_array(Array *array, void *item);
void *get_array_at(Array *array, size_t index);
void *pop_array(Array *array);

char *get_token_string(Token *token);
void print_tokens(Token *token);
char *get_node_string(Node *node);
void print_node_tree(Node *node, int level, const char *prefix);
char *get_result_string(Result *result);
void print_result(Result *result);

#endif
