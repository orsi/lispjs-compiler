#include "./roxanne.h"
#include <math.h>
#include <stdio.h>

void print_result(Result *result) {
  if (result->type == RESULT_NONE) {
    printf("No result\n");
    return;
  }
  if (result->type == RESULT_INTEGER) {
    printf("%d\n", result->int_value);
    return;
  }
  if (result->type == RESULT_DOUBLE) {
    printf("%f\n", result->double_value);
    return;
  }
}

Result *evaluate_binary_expression(Result *result, char symbol, Node *left_node,
                                   Node *right_node) {
  Result *left_result = malloc(sizeof(Result));
  Result *right_result = malloc(sizeof(Result));

  // get left and right values
  if (left_node->type == NODE_BINARY_EXPRESSION) {
    evaluate_binary_expression(left_result, left_node->symbol_value,
                               left_node->left, left_node->right);
  } else if (left_node->type == NODE_DOUBLE) {
    left_result->type = RESULT_DOUBLE;
    left_result->double_value = left_node->double_value;
  } else if (left_node->type == NODE_INTEGER) {
    left_result->type = RESULT_INTEGER;
    left_result->int_value = left_node->int_value;
  }

  if (right_node->type == NODE_BINARY_EXPRESSION) {
    evaluate_binary_expression(right_result, right_node->symbol_value,
                               right_node->left, right_node->right);
  } else if (right_node->type == NODE_DOUBLE) {
    right_result->type = RESULT_DOUBLE;
    right_result->double_value = right_node->double_value;
  } else if (right_node->type == NODE_INTEGER) {
    right_result->type = RESULT_INTEGER;
    right_result->double_value = right_node->double_value;
  }

  if (symbol == '+') {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_INTEGER;
      result->int_value = left_result->int_value + right_result->int_value;
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->int_value + right_result->double_value;
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->double_value + right_result->int_value;
    } else {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->double_value + right_result->double_value;
    }
  } else if (symbol == '-') {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_INTEGER;
      result->int_value = left_result->int_value - right_result->int_value;
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->int_value - right_result->double_value;
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->double_value - right_result->int_value;
    } else {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->double_value - right_result->double_value;
    }
  } else if (symbol == '*') {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_INTEGER;
      result->int_value = left_result->int_value * right_result->int_value;
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->int_value * right_result->double_value;
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->double_value * right_result->int_value;
    } else {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->double_value * right_result->double_value;
    }
  } else if (symbol == '/') {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          (double)left_result->int_value / (double)right_result->int_value;
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          (double)left_result->int_value / right_result->double_value;
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->double_value / (double)right_result->int_value;
    } else {
      result->type = RESULT_DOUBLE;
      result->double_value =
          left_result->double_value / right_result->double_value;
    }
  } else /* modulo */ {
    if (left_result->type == RESULT_INTEGER &&
        right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          fmod(left_result->int_value, right_result->int_value);
    } else if (left_result->type == RESULT_INTEGER &&
               right_result->type == RESULT_DOUBLE) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          fmod(left_result->int_value, right_result->double_value);
    } else if (left_result->type == RESULT_DOUBLE &&
               right_result->type == RESULT_INTEGER) {
      result->type = RESULT_DOUBLE;
      result->double_value =
          fmod(left_result->double_value, right_result->int_value);
    } else {
      result->type = RESULT_DOUBLE;
      result->double_value =
          fmod(left_result->double_value, right_result->double_value);
    }
  }

  return result;
}

Result *evaluate_node(Node *node) {
  Result *result = malloc(sizeof(Result));

  if (node->type == NODE_BINARY_EXPRESSION) {
    evaluate_binary_expression(result, node->symbol_value, node->left,
                               node->right);
  } else if (node->type == NODE_DOUBLE) {
    result->type = RESULT_DOUBLE;
    result->double_value = node->double_value;
  } else if (node->type == NODE_INTEGER) {
    result->type = RESULT_INTEGER;
    result->int_value = node->int_value;
  } else {
    result->type = RESULT_NONE;
  }

  return result;
}