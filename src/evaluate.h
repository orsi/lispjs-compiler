#ifndef EVALUATE_H
#define EVALUATE_H

#include "parse.h"
#include "utils.h"
#include <stdbool.h>

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
  };
} Result;
char *stringify_result(Result *result);
void print_result(Result *result);
Result *evaluate_assignment_expression(Result *result, Node *left_node,
                                       Node *right_node);
Result *evaluate_binary_expression(Result *result, char *operator_symbol,
                                   Node *left_node, Node *right_node);
Result *evaluate(Node *node);
#endif
