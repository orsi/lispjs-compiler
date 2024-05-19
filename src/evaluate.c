#include "./roxanne.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

char *get_result_string(Result *result) {
  char string[128];
  int length;
  switch (result->type) {
  case (RESULT_NONE): {
    length = sprintf(string, "result:none");
    break;
  }
  case (RESULT_INTEGER): {
    length = sprintf(string, "result:integer, %d", result->value_int);
    break;
  }
  case (RESULT_DOUBLE): {
    length = sprintf(string, "result:double, %f", result->value_double);
    break;
  }
  case RESULT_STRING: {
    length = sprintf(string, "result:string, %.*s", result->value_string.length,
                     result->value_string.value);
    break;
  }
  }

  char *result_string = malloc(length);
  memcpy(result_string, string, length);
  return result_string;
}

void print_result(Result *result) { printf("%s", get_result_string(result)); }

Result *evaluate_assignment_expression(Result *result, Node *left_node,
                                       Node *right_node) {
  Result *right_result = malloc(sizeof(Result));

  // get right value
  if (right_node->type == NODE_EXPRESSION_BINARY) {
    evaluate_binary_expression(right_result, right_node->expression_symbol,
                               right_node->left, right_node->right);
  } else if (right_node->type == NODE_VALUE_DOUBLE) {
    right_result->type = RESULT_DOUBLE;
    right_result->value_double = right_node->value_double;
  } else if (right_node->type == NODE_VALUE_INTEGER) {
    right_result->type = RESULT_INTEGER;
    right_result->value_double = right_node->value_double;
  }

  return result;
}

Result *evaluate_binary_expression(Result *result, char symbol, Node *left_node,
                                   Node *right_node) {
  Result *left_result = malloc(sizeof(Result));
  Result *right_result = malloc(sizeof(Result));

  // get left and right values
  if (left_node->type == NODE_EXPRESSION_BINARY) {
    evaluate_binary_expression(left_result, left_node->expression_symbol,
                               left_node->left, left_node->right);
  } else if (left_node->type == NODE_VALUE_DOUBLE) {
    left_result->type = RESULT_DOUBLE;
    left_result->value_double = left_node->value_double;
  } else if (left_node->type == NODE_VALUE_INTEGER) {
    left_result->type = RESULT_INTEGER;
    left_result->value_int = left_node->value_int;
  }

  if (right_node->type == NODE_EXPRESSION_BINARY) {
    evaluate_binary_expression(right_result, right_node->expression_symbol,
                               right_node->left, right_node->right);
  } else if (right_node->type == NODE_VALUE_DOUBLE) {
    right_result->type = RESULT_DOUBLE;
    right_result->value_double = right_node->value_double;
  } else if (right_node->type == NODE_VALUE_INTEGER) {
    right_result->type = RESULT_INTEGER;
    right_result->value_double = right_node->value_double;
  }

  if (symbol == '+') {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_INTEGER;
      result->value_int = left_result->value_int + right_result->value_int;
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_int + right_result->value_double;
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_double + right_result->value_int;
    } else {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_double + right_result->value_double;
    }
  } else if (symbol == '-') {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_INTEGER;
      result->value_int = left_result->value_int - right_result->value_int;
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_int - right_result->value_double;
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_double - right_result->value_int;
    } else {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_double - right_result->value_double;
    }
  } else if (symbol == '*') {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_INTEGER;
      result->value_int = left_result->value_int * right_result->value_int;
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_int * right_result->value_double;
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_double * right_result->value_int;
    } else {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_double * right_result->value_double;
    }
  } else if (symbol == '/') {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          (double)left_result->value_int / (double)right_result->value_int;
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          (double)left_result->value_int / right_result->value_double;
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_double / (double)right_result->value_int;
    } else {
      result->type = RESULT_DOUBLE;
      result->value_double =
          left_result->value_double / right_result->value_double;
    }
  } else /* modulo */ {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          fmod(left_result->value_int, right_result->value_int);
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          fmod(left_result->value_int, right_result->value_double);
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->value_double =
          fmod(left_result->value_double, right_result->value_int);
    } else {
      result->type = RESULT_DOUBLE;
      result->value_double =
          fmod(left_result->value_double, right_result->value_double);
    }
  }

  return result;
}

Result *evaluate(Node *node) {
  Result *result = malloc(sizeof(Result));

  switch (node->type) {
  case (NODE_EXPRESSION_BINARY): {
    evaluate_binary_expression(result, node->expression_symbol, node->left,
                               node->right);
    break;
  }
  case (NODE_EXPRESSION_ASSIGNMENT): {
    evaluate_assignment_expression(result, node->left, node->right);
    break;
  }
  case (NODE_VALUE_DOUBLE): {
    result->type = RESULT_DOUBLE;
    result->value_double = node->value_double;
    break;
  }
  case (NODE_VALUE_INTEGER): {
    result->type = RESULT_INTEGER;
    result->value_int = node->value_int;
    break;
  }
  case NODE_VALUE_STRING: {
    result->type = RESULT_STRING;
    result->value_string = node->value_string;
    break;
  }
  default: {
    result->type = RESULT_NONE;
    break;
  }
  }

  return result;
}