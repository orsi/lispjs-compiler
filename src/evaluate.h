#ifndef EVALUATE_H
#define EVALUATE_H

#include "./parse.h"

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
#endif