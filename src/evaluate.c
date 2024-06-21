#include "roxanne.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

Result *evaluate_assignment_expression(Result *result, Node *left_node,
                                       Node *right_node) {
  Result *right_result = malloc(sizeof(Result));
  if (right_result == NULL) {
    printf("Error: malloc right_result\n");
    exit(1);
  }

  // get right value
  if (right_node->type == NODE_EXPRESSION_BINARY) {
    evaluate_binary_expression(right_result, right_node->operator_symbol,
                               right_node->left, right_node->right);
  } else if (right_node->type == NODE_LITERAL_NUMBER) {
    right_result->type = RESULT_NUMBER;
    right_result->number = right_node->number;
  }

  return result;
}

Result *evaluate_binary_expression(Result *result, char *operator_symbol,
                                   Node *left_node, Node *right_node) {
  Result *left_result = malloc(sizeof(Result));
  if (left_result == NULL) {
    printf("Error: malloc left_result\n");
    exit(1);
  }
  Result *right_result = malloc(sizeof(Result));
  if (right_result == NULL) {
    printf("Error: malloc right_result\n");
    exit(1);
  }

  // get left and right values
  if (left_node->type == NODE_EXPRESSION) {
    // left_result = evaluate(left_node->expression);
  } else if (left_node->type == NODE_EXPRESSION_BINARY) {
    evaluate_binary_expression(left_result, left_node->operator_symbol,
                               left_node->left, left_node->right);
  } else if (left_node->type == NODE_LITERAL_NUMBER) {
    left_result->type = RESULT_NUMBER;
    left_result->number = left_node->number;
  }

  if (right_node->type == NODE_EXPRESSION) {
    // right_result = evaluate(right_node->expression);
  } else if (right_node->type == NODE_EXPRESSION_BINARY) {
    evaluate_binary_expression(right_result, right_node->operator_symbol,
                               right_node->left, right_node->right);
  } else if (right_node->type == NODE_LITERAL_NUMBER) {
    right_result->type = RESULT_NUMBER;
    right_result->number = right_node->number;
  }

  if (starts_with(operator_symbol, "+")) {
    result->type = RESULT_NUMBER;
    result->number = left_result->number + right_result->number;
  } else if (starts_with(operator_symbol, "-")) {
    result->type = RESULT_NUMBER;
    result->number = left_result->number - right_result->number;
  } else if (starts_with(operator_symbol, "*")) {
    result->type = RESULT_NUMBER;
    result->number = left_result->number * right_result->number;
  } else if (starts_with(operator_symbol, "/")) {
    result->type = RESULT_NUMBER;
    result->number = left_result->number / right_result->number;
  } else /* modulo */ {
    result->type = RESULT_NUMBER;
    result->number = fmod(left_result->number, right_result->number);
  }

  return result;
}

Result *evaluate(Node *node) {
  Result *result = malloc(sizeof(Result));
  if (result == NULL) {
    printf("Error: malloc result\n");
    exit(1);
  }

  switch (node->type) {
  case NODE_EXPRESSION: {
    // result = evaluate(node->expression);
    break;
  }
  case (NODE_EXPRESSION_BINARY): {
    evaluate_binary_expression(result, node->operator_symbol, node->left,
                               node->right);
    break;
  }
  case (NODE_EXPRESSION_ASSIGNMENT): {
    evaluate_assignment_expression(result, node->left, node->right);
    break;
  }
  case NODE_ARRAY: {
    // result->type = RESULT_ARRAY;
    // result->array = *node->array;
    break;
  }
  case NODE_LITERAL_BOOLEAN: {
    result->type = RESULT_BOOLEAN;
    result->boolean = node->boolean;
    break;
  }
  case (NODE_LITERAL_NUMBER): {
    result->type = RESULT_NUMBER;
    result->number = node->number;
    break;
  }
  case NODE_LITERAL_STRING: {
    result->type = RESULT_STRING;
    result->string = *node->string;
    break;
  }
  case NODE_LITERAL_STRING_TEMPLATE: {
    String string = {0};
    string.length = 0;
    string.value = malloc(sizeof(char *));

    Node *current_node = node->parts;
    while (current_node) {
      Result *part_result = evaluate(current_node);
      if (part_result->type == RESULT_STRING) {
        string.length += part_result->string.length;
        string.value = realloc(string.value, string.length);
        strncat(string.value, part_result->string.value,
                part_result->string.length);
        current_node = current_node->next;
        continue;
      }

      if (part_result->type == RESULT_BOOLEAN) {
        char buffer[128];
        size_t l =
            sprintf(buffer, "%s", part_result->boolean ? "true" : "false");
        string.length += l;
        string.value = realloc(string.value, string.length);
        strncat(string.value, buffer, l);
        current_node = current_node->next;
        continue;
      }

      char buffer[128];
      size_t l = sprintf(buffer, "%g", part_result->number);
      string.length += l;
      string.value = realloc(string.value, string.length);
      strncat(string.value, buffer, l);
      current_node = current_node->next;
    }

    result->type = RESULT_STRING;
    result->string = string;
    break;
  }
  case NODE_BLOCK:
  case NODE_STATEMENT_CONDITIONAL:
  case NODE_LITERAL_IDENTIFIER:
  case NODE_LITERAL_OBJECT: {
    result->type = RESULT_NONE;
    result->string = (String){0, (char *)""};
    break;
  }
  }

  return result;
}
