#ifndef PARSE_H
#define PARSE_H

#include "lex.h"
#include "utils.h"

typedef struct Node Node;
typedef struct {
  Node *parameters;
  Node *block;
} Function;
enum NodeType {
  NODE_PROGRAM,
  NODE_EXPRESSION,
  NODE_EXPRESSION_ASSIGNMENT,
  NODE_EXPRESSION_BINARY,
  NODE_ARRAY,
  NODE_BLOCK,
  NODE_CONDITIONAL,
  NODE_FUNCTION,
  NODE_LITERAL_BOOLEAN,
  NODE_LITERAL_IDENTIFIER,
  NODE_LITERAL_NUMBER,
  NODE_LITERAL_STRING,
  NODE_LITERAL_STRING_TEMPLATE,
};
struct Node {
  enum NodeType type;
  Node *next;
  Token *start;
  // end token is exclusive and was not parsed
  Token *end;
  // n.b. C11 anonymous union/struct
  union {
    // literals
    bool boolean;
    char *identifier;
    struct {
      int base;
      double number;
    };
    String *string;
    // expression, array, block, object, program
    Node *body;
    // expression - assignment
    struct {
      Node *variable;
      Node *value;
    };
    // expression - binary
    struct {
      char *operator_symbol;
      Node *left;
      Node *right;
    };
    // function
    struct {
      Node *parameters;
      Node *block;
    };
    // string templates
    struct {
      Node *parts;
    };
    // todo
    // conditional
    struct {
      Node *condition;
      Node *if_then;
      Node *if_else;
    };
  };
};
char *stringify_node_value(Node *node);
char *stringify_node(Node *node);
void print_nodes(Node *node, int level, const char *prefix);
void print_program(Node *nodes);
char *get_operator(Token *tokens);
Node *create_node(enum NodeType type, Token *start, Token *end);
int get_operator_precedence(char *s);
Node *parse(Token *tokens, Node *last_node);
#endif
