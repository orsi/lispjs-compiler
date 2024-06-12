#include "roxanne.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *keywords[] = {"if",    "import", "else", "export",
                                 "false", "return", "true"};

int is_keyword(char *word, int length) {
  int is_match = 0;
  for (size_t i = 0; i < sizeof(keywords) / sizeof(*keywords); i++) {
    const char *key = keywords[i];
    is_match = strncmp(word, key, length) == 0; // i hate this
    if (is_match)
      break;
  }
  return is_match;
}

char *get_operator(Token *tokens) {
  Token *current_token = tokens;
  size_t length = 0;

  while (current_token && length < current_token->length &&
         ispunct(current_token->value[0])) {
    length += 1;
    current_token = tokens->next;
  }

  char *operator_symbol = malloc(sizeof(char) * length);
  strncpy(operator_symbol, tokens->value, length);
  return operator_symbol;
}

int get_operator_precedence(char *s) {
  if (starts_with(s, "*") || starts_with(s, "/") || starts_with(s, "%%")) {
    return 2;
  } else if (starts_with(s, "+") || starts_with(s, "-")) {
    return 1;
  } else {
    return 0;
  }
}

Node *create_node(enum NodeType type, void *value) {
  Node *node = malloc(sizeof(Node));
  if (node == NULL) {
    printf("Error malloc node.\n");
    exit(1);
  }

  node->type = type;
  switch (node->type) {
  case NODE_STATEMENT_CONDITIONAL: {
    NodeConditional *conditional_statement = value;
    node->condition = conditional_statement->expression;
    node->if_then = conditional_statement->if_then;
    node->if_else = conditional_statement->if_else;
    break;
  }
  case NODE_EXPRESSION_ASSIGNMENT:
  case NODE_EXPRESSION_BINARY: {
    NodeBinary *binary_expression = value;
    node->operator_symbol = binary_expression->operator_symbol;
    node->left = binary_expression->left;
    node->right = binary_expression->right;
    break;
  }
  case NODE_ARRAY:
  case NODE_BLOCK:
  case NODE_EXPRESSION:
  case NODE_PROGRAM:
    node->body = value;
    break;
  case NODE_LITERAL_BOOLEAN:
    node->boolean = *(bool *)value;
    break;
  case NODE_LITERAL_FUNCTION:
    node->function = value;
    break;
  case NODE_LITERAL_IDENTIFIER:
    node->identifier = value;
    break;
  case NODE_LITERAL_STRING:
    node->string = value;
    break;
  case NODE_LITERAL_STRING_TEMPLATE:
    node->string_template_parts = value;
    break;
  case NODE_LITERAL_NUMBER:
    node->number = *(double *)value;
    break;
  case NODE_LITERAL_OBJECT:
    node->object = value;
    break;
  case NODE_STATEMENT_MULTI:
    node->statements = value;
    break;
  }

  return node;
}

Node *parse(Token *token, Node *parent_node) {
  Node head = {0};
  Node *current_node = &head;
  Node *last_node = NULL;

  Token *current_token = token;
  while (current_token && current_token->type != TOKEN_END_OF_FILE) {
    // statement terminator
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, ";")) {
      current_node = current_node->next = last_node;
      last_node = NULL;
      current_token = current_token->next;
      continue;
    }

    // elision
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, ",")) {
      current_node = current_node->next = last_node;
      current_token = current_token->next;
      continue;
    }

    // Literal - Numbers
    //
    // We want to make sure to clean up the original token value, as it may
    // contain space and underscore separators.
    if (current_token->type == TOKEN_NUMBER) {
      char normalizedNumber[current_token->length + 1];
      char *current_value = current_token->value;
      size_t i = 0;
      while ((size_t)(current_value - current_token->value) <
             current_token->length) {
        if (!starts_with(current_value, " ") &&
            !starts_with(current_value, "_")) {
          normalizedNumber[i] = current_value[0];
          i++;
        }
        current_value++;
      }
      normalizedNumber[i] = '\0';
      double value = atof(normalizedNumber);
      last_node = create_node(NODE_LITERAL_NUMBER, &value);
      current_token = current_token->next;
      continue;
    }

    // literal binary number
    if (current_token->type == TOKEN_NUMBER_ALTERNATIVE_BASE &&
        starts_with(current_token->value, "#")) {
      // strip all spaces from value and provide null terminator
      char normalizedNumber[current_token->length + 1];
      char *position = current_token->value;
      position++; // # skip
      int base = position[0] == 'b' ? 2 : position[0] == 'o' ? 8 : 16;
      position++; // b/h/o skip
      size_t i = 0;
      while ((size_t)(position - current_token->value) <
             current_token->length) {
        if (position[0] != ' ') {
          normalizedNumber[i] = position[0];
          i++;
        }
        position++;
      }
      normalizedNumber[i] = '\0';
      double value = strtol(normalizedNumber, NULL, base);
      last_node = create_node(NODE_LITERAL_NUMBER, &value);
      current_token = current_token->next;
      continue;
    }

    // literal string
    if (current_token->type == TOKEN_STRING) {
      String *value = malloc(sizeof(String));
      if (value == NULL) {
        printf("Error: cannot malloc value\n");
        exit(1);
      }
      value->length = current_token->length;
      value->value = current_token->value;
      last_node = create_node(NODE_LITERAL_STRING, value);
      current_token = current_token->next;
      continue;
    }



    // TODO: Now to rework string interpolation better without
    // having to use janky arrays



    // literal string interpolation
    if (current_token->type == TOKEN_STRING_TEMPLATE_START) {
      Array *node_template_parts = create_array();

      current_token = current_token->next; // skip template start token
      while (current_token &&
             current_token->type != TOKEN_STRING_TEMPLATE_END) {
        if (current_token->type == TOKEN_STRING_TEMPLATE_PART_START) {
          current_token = current_token->next; // skip template part start token
          Node *current_root = NULL;
          while (current_token &&
                 current_token->type != TOKEN_STRING_TEMPLATE_PART_END) {
            current_root = parse(current_token, current_root);
          }
          push_array(node_template_parts, current_root);
          current_token = current_token->next; // skip template part end token
        } else {
          Node *node_literal_string = parse(current_token, NULL);
          push_array(node_template_parts, node_literal_string);
        }
      }

      last_node =
          create_node(NODE_LITERAL_STRING_TEMPLATE, node_template_parts);
      current_token = current_token->next;
      continue;
    }

    // identifier literal: true
    if (current_token->type == TOKEN_IDENTIFIER &&
        starts_with(current_token->value, "true")) {
      bool value = true;
      last_node = create_node(NODE_LITERAL_BOOLEAN, &value);
      current_token = current_token->next;
      continue;
    }

    // identifier literal: false
    if (current_token->type == TOKEN_IDENTIFIER &&
        starts_with(current_token->value, "false")) {
      bool value = false;
      last_node = create_node(NODE_LITERAL_BOOLEAN, &value);
      current_token = current_token->next;
      continue;
    }

    // identifier keyword: if
    if (current_token->type == TOKEN_IDENTIFIER &&
        starts_with(current_token->value, "if")) {
      NodeConditional value;

      // conditional
      Token sub_head = *current_token->next;
      Token *current_sub_token = &sub_head;
      while (current_sub_token->next &&
             !starts_with(current_sub_token->next->value, "{")) {
        current_sub_token = current_sub_token->next;
      }
      current_token = current_sub_token->next;
      current_sub_token->next = NULL;
      value.expression = parse(&sub_head, NULL);

      // then block
      sub_head = *current_token;
      current_sub_token = &sub_head;
      size_t matching_token_count = 0; // for counting matching tokens
      while (current_sub_token) {
        current_sub_token = current_sub_token->next;

        if (starts_with(current_sub_token->value, "{")) {
          matching_token_count = matching_token_count + 1;
        } else if (starts_with(current_sub_token->value, "}") &&
                   matching_token_count > 0) {
          matching_token_count = matching_token_count - 1;
        } else if (starts_with(current_sub_token->value, "}") &&
                   matching_token_count == 0) {
          break;
        }
      }
      current_token = current_sub_token->next;
      current_sub_token->next = NULL;
      value.if_then = parse(&sub_head, NULL);

      // else block
      if (current_token->type == TOKEN_IDENTIFIER &&
          starts_with(current_token->value, "else")) {
        sub_head = *current_token->next;
        current_sub_token = &sub_head;
        matching_token_count = 0; // for counting matching tokens
        while (current_sub_token &&
               (!starts_with(current_sub_token->value, "}") &&
                matching_token_count == 0)) {
          current_sub_token = current_sub_token->next;

          if (starts_with(current_sub_token->value, "{")) {
            matching_token_count = matching_token_count + 1;
          } else if (starts_with(current_sub_token->value, "}") &&
                     matching_token_count > 0) {
            matching_token_count = matching_token_count - 1;
          }
        }
        current_token = current_sub_token->next;
        current_sub_token->next = NULL;
        value.if_else = parse(&sub_head, NULL);
      }

      last_node = create_node(NODE_STATEMENT_CONDITIONAL, &value);
      continue;
    }

    // identifier
    if (current_token->type == TOKEN_IDENTIFIER) {
      size_t size = current_token->length + 1;
      char *value = malloc(sizeof(char *) * size);
      if (value == NULL) {
        printf("Error: cannot malloc value\n");
        exit(1);
      }
      value = strncpy(value, current_token->value, current_token->length);
      value[size] = '\0';
      last_node = create_node(NODE_LITERAL_IDENTIFIER, value);
      current_token = current_token->next;
      continue;
    }

    // function
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, "=>") && last_node != NULL &&
        last_node->type == NODE_EXPRESSION) {
      current_token = (current_token)->next; // skip =
      current_token = (current_token)->next; // skip >

      Token sub_head = *current_token;
      Token *current_sub_token = &sub_head;
      size_t matching_token_count = 0; // for counting matching tokens
      while (current_sub_token &&
             (!starts_with(current_sub_token->value, "}") &&
              matching_token_count == 0)) {
        current_sub_token = current_sub_token->next;

        if (starts_with(current_sub_token->value, "{")) {
          matching_token_count = matching_token_count + 1;
        } else if (starts_with(current_sub_token->value, "}") &&
                   matching_token_count > 0) {
          matching_token_count = matching_token_count - 1;
        };
      }
      current_token = current_sub_token->next; // skip }
      current_sub_token->next = NULL;

      Node *block = parse(&sub_head, NULL);
      Function *function = malloc(sizeof(*function));
      function->parameters = last_node;
      function->block = block;
      last_node = create_node(NODE_LITERAL_FUNCTION, function);
      continue;
    }

    // array
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, "[")) {
      Token sub_head = *current_token->next;
      Token *current_sub_token = &sub_head;
      current_token = current_token->next; // skip opening [
      size_t matching_token_count = 0;     // for counting matching tokens
      while (current_token) {
        if (starts_with(current_token->value, "[")) {
          matching_token_count = matching_token_count + 1;
        } else if (starts_with(current_token->value, "]") &&
                   matching_token_count > 0) {
          matching_token_count = matching_token_count - 1;
        } else if (starts_with(current_token->value, "]") &&
                   matching_token_count == 0) {
          break;
        }
        current_sub_token = current_sub_token->next;
        current_token = current_sub_token->next;
      }
      current_token = current_token->next; // skip ]
      current_sub_token->next = NULL;
      last_node = create_node(NODE_ARRAY, parse(&sub_head, NULL));
      continue;
    }

    // block
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, "{")) {
      Token sub_head = *current_token;
      Token *current_sub_token = &sub_head;
      size_t matching_token_count = 0; // for counting matching tokens
      while (current_sub_token->next &&
             (!starts_with(current_sub_token->next->value, "}") &&
              matching_token_count == 0)) {
        current_sub_token = current_sub_token->next;

        if (starts_with(current_sub_token->value, "{")) {
          matching_token_count = matching_token_count + 1;
        } else if (starts_with(current_sub_token->value, "}") &&
                   matching_token_count > 0) {
          matching_token_count = matching_token_count - 1;
        };
      }
      current_token = current_sub_token->next; // skip }
      current_token = current_token->next;     // skip }
      current_sub_token->next = NULL;
      last_node = create_node(NODE_BLOCK, parse(sub_head.next, NULL));
      continue;
    }

    // expression
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, "(")) {
      Token sub_head = *current_token;
      Token *current_sub_token = &sub_head;
      size_t matching_token_count = 0; // for counting matching tokens
      while (current_sub_token->next &&
             (!starts_with(current_sub_token->next->value, ")") &&
              matching_token_count == 0)) {
        current_sub_token = current_sub_token->next;

        if (starts_with(current_sub_token->value, "(")) {
          matching_token_count = matching_token_count + 1;
        } else if (starts_with(current_sub_token->value, ")") &&
                   matching_token_count > 0) {
          matching_token_count = matching_token_count - 1;
        }
      }
      current_token = current_sub_token->next; // skip )
      current_token = current_token->next;     // skip )
      current_sub_token->next = NULL;

      Node *body = parse(sub_head.next, NULL);
      last_node = create_node(NODE_EXPRESSION, body);
      continue;
    }

    // expression - assignment
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, ":")) {
      char *operator_symbol = get_operator(current_token);

      current_token = current_token->next; // skip operator
      Token sub_head = *current_token;
      Token *current_sub_token = &sub_head;
      while (current_token->next &&
             !starts_with(current_token->next->value, ";") &&
             current_token->next->type != TOKEN_END_OF_FILE) {
        current_sub_token = current_sub_token->next;
        current_token = current_token->next;
      }
      current_sub_token->next = NULL;
      current_token = current_token->next;

      NodeBinary value = {
          .operator_symbol = operator_symbol, .left = last_node, .right = NULL};
      last_node = create_node(NODE_EXPRESSION_ASSIGNMENT, &value);
      last_node->right = parse(&sub_head, last_node);
      continue;
    }

    // expression - binary
    if (current_token->type == TOKEN_SYMBOL &&
        (starts_with(current_token->value, "+") ||
         starts_with(current_token->value, "-") ||
         starts_with(current_token->value, "/") ||
         starts_with(current_token->value, "*") ||
         starts_with(current_token->value, "%%") ||
         starts_with(current_token->value, "<") ||
         starts_with(current_token->value, ">") ||
         starts_with(current_token->value, "<=") ||
         starts_with(current_token->value, ">="))) {
      char *operator_symbol = get_operator(current_token);
      current_token = current_token->next; // skip operator
      Token sub_head = *current_token;
      Token *current_sub_token = &sub_head;
      while (current_token->next &&
             !starts_with(current_token->next->value, ";") &&
             current_token->next->type != TOKEN_END_OF_FILE) {
        current_sub_token = current_sub_token->next;
        current_token = current_token->next;
      }
      current_sub_token->next = NULL;

      if (parent_node != NULL && parent_node->type == NODE_EXPRESSION_BINARY &&
          get_operator_precedence(parent_node->operator_symbol) <
              get_operator_precedence(operator_symbol)) {
        // when we have multiple binary expressions in a row, we
        // need to compare their operator precedence, and if the current
        // binary expression has greater precendence, we have to steal
        // the right operand from the lesser precedence, and replace
        // it with this binary expression
        NodeBinary value = {.operator_symbol = operator_symbol,
                            .left = last_node,
                            .right = NULL};
        last_node = create_node(NODE_EXPRESSION_BINARY, &value);
        last_node->right = parse(&sub_head, last_node);
        current_token = current_token->next;
        continue;
      } else {
        NodeBinary value = {.operator_symbol = operator_symbol,
                            .left = last_node,
                            .right = NULL};
        last_node = create_node(NODE_EXPRESSION_BINARY, &value);
        last_node->right = parse(&sub_head, last_node);
        current_token = current_token->next;
        continue;
      }
    }
  }

  // end of file
  if (last_node != NULL) {
    current_node->next = last_node;
  }

  return head.next;
}
