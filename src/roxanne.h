#ifndef ROXANNE_H
#define ROXANNE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
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
  TOKEN_STRING_TEMPLATE_START,
  TOKEN_STRING_TEMPLATE_END,
  TOKEN_STRING_TEMPLATE_PART_START,
  TOKEN_STRING_TEMPLATE_PART_END,
  TOKEN_SYMBOL,
  TOKEN_END_OF_FILE,
};
typedef struct Token Token;
struct Token {
  enum TokenType type;
  char *start;
  char *end;
  size_t length;
  char *value;
  Token *next;
};

Token *create_token(enum TokenType type, char *start, char *end, int length,
                    char *value);
Token *lex(const char *start);

// parsing
typedef struct Node Node;
typedef struct {
  Node *parameters;
  Node *block;
} Function;
enum NodeType {
  NODE_EXPRESSION,
  NODE_EXPRESSION_ASSIGNMENT,
  NODE_EXPRESSION_BINARY,
  NODE_STATEMENT_MULTI,
  NODE_STATEMENT_BLOCK,
  NODE_STATEMENT_CONDITIONAL,
  NODE_LITERAL_ARRAY,
  NODE_LITERAL_BOOLEAN,
  NODE_LITERAL_FUNCTION,
  NODE_LITERAL_IDENTIFIER,
  NODE_LITERAL_NUMBER,
  NODE_LITERAL_OBJECT,
  NODE_LITERAL_STRING,
  NODE_LITERAL_STRING_TEMPLATE,
};
struct Node {
  enum NodeType type;
  Node *left;
  Node *right;
  union {
    bool boolean;
    char *operator_symbol;
    char *identifier;
    double number;
    String *string;
    Node *expression;
    Function *function;
    Array *array;
    Array *string_template_parts;
    Array *statements;
    Array *object;
  };
};
typedef struct {
  Array *statements;
} Program;

int is_keyword(char *word, int length);
char *get_operator(Token *tokens);
Node *create_node(enum NodeType type, void *value, Node *left, Node *right);
int get_operator_precedence(char *s);
Node *parse_expression(Token **tokens, Node *last_node);
Program *parse(Token *tokens);

// evaluate
enum ResultType {
  RESULT_NONE,
  RESULT_ARRAY,
  RESULT_BOOLEAN,
  RESULT_NUMBER,
  RESULT_STRING
};
typedef struct {
  enum ResultType type;
  union {
    bool boolean;
    double number;
    String string;
    Array array;
  };
} Result;
Result *evaluate_assignment_expression(Result *result, Node *left_node,
                                       Node *right_node);
Result *evaluate_binary_expression(Result *result, char *operator_symbol,
                                   Node *left_node, Node *right_node);
Result *evaluate(Node *node);

// utils
char *string_duplicate(char *destination, size_t length);
bool starts_with(const char *a, const char *b);

char *read_filepath(const char *filepath);

Array *create_array(void);
Array *push_array(Array *array, void *item);
void *get_array_item_at(Array *array, size_t index);
void *pop_array(Array *array);

char *stringify_node_value(Node *node);
char *stringify_array(Array *array);
char *stringify_token(Token *token);
char *stringify_node(Node *node);
char *stringify_result(Result *result);

void print_tokens(Token *token);
void print_node_tree(Node *node, int level, const char *prefix);
void print_result(Result *result);
void print_program(Program *program);

#endif
