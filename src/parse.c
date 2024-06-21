#include "roxanne.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Node *create_node(enum NodeType type, Token *start, Token *end) {
  Node *node = malloc(sizeof(Node));
  if (node == NULL) {
    printf("Error malloc node.\n");
    exit(1);
  }

  node->type = type;
  node->start = start;

  if (end != NULL) {
    node->end = end;
  } else if (start != NULL) {
    node->end = start->next;
  }

  return node;
}

Node *parse_node(Token *token, Node *last_node) {
  // literal number
  if (token->type == TOKEN_NUMBER) {
    char normalizedNumber[token->length + 1];
    char *current_value = token->value;
    size_t i = 0;
    while ((size_t)(current_value - token->value) < token->length) {
      if (!starts_with(current_value, " ") &&
          !starts_with(current_value, "_")) {
        normalizedNumber[i] = current_value[0];
        i++;
      }
      current_value++;
    }
    normalizedNumber[i] = '\0';
    Node *node = create_node(NODE_LITERAL_NUMBER, token, NULL);
    node->base = 10;
    node->number = atof(normalizedNumber);
    return node;
  }

  // literal binary number
  if (token->type == TOKEN_NUMBER_ALTERNATIVE_BASE &&
      starts_with(token->value, "#")) {
    char normalizedNumber[token->length + 1];
    char *position = token->value;
    position++; // # skip
    position++; // b/h/o skip
    size_t i = 0;
    while ((size_t)(position - token->value) < token->length) {
      if (position[0] != ' ') {
        normalizedNumber[i] = position[0];
        i++;
      }
      position++;
    }
    normalizedNumber[i] = '\0';

    int base = position[0] == 'b' ? 2 : position[0] == 'o' ? 8 : 16;
    double number = strtol(normalizedNumber, NULL, base);
    Node *node = create_node(NODE_LITERAL_NUMBER, token, NULL);
    node->base = base;
    node->number = number;
    return node;
  }

  // literal string
  if (token->type == TOKEN_STRING) {
    Node *node = create_node(NODE_LITERAL_STRING, token, NULL);
    node->string = malloc(sizeof(String));
    if (node->string == NULL) {
      printf("Error: cannot malloc value\n");
      exit(1);
    }
    node->string->length = token->length;
    node->string->value = token->value;
    return node;
  }

  // keywords
  if (token->type == TOKEN_KEYWORD) {
    Node *node = create_node(NODE_LITERAL_BOOLEAN, token, NULL);
    if (starts_with(token->value, "true")) {
      node->type = NODE_LITERAL_BOOLEAN;
      node->boolean = true;
    } else if (starts_with(token->value, "false")) {
      node->type = NODE_LITERAL_BOOLEAN;
      node->boolean = false;
    } else {
      printf("Keyword not implemented: %.*s", (int)token->length, token->start);
      exit(1);
    }

    return node;
  }

  // identifier
  if (token->type == TOKEN_IDENTIFIER) {
    Node *node = create_node(NODE_LITERAL_IDENTIFIER, token, NULL);
    size_t size = token->length + 1;
    node->identifier = malloc(size);
    if (node->identifier == NULL) {
      printf("Error: cannot malloc value\n");
      exit(1);
    }
    node->identifier = strncpy(node->identifier, token->value, token->length);
    node->identifier[size] = '\0';
    return node;
  }

  // literal string template
  if (token->type == TOKEN_STRING_TEMPLATE_START) {
    Node *node = create_node(NODE_LITERAL_STRING_TEMPLATE, token, NULL);

    token = token->next; // skip TOKEN_STRING_TEMPLATE_START

    Node head = {0};
    Node *list_head = &head;
    Node *current_node = NULL;
    size_t matching_token_count = 0;
    while (token && (token->type != TOKEN_STRING_TEMPLATE_END ||
                     matching_token_count > 0)) {

      // keeps track of nested string templates so we don't break early
      if (token->type == TOKEN_STRING_TEMPLATE_START) {
        matching_token_count++;
      } else if (token->type == TOKEN_STRING_TEMPLATE_END) {
        matching_token_count--;
      }

      // advance past string template markers and move the node_list
      // forward if it exists
      if (token->type == TOKEN_STRING_TEMPLATE_START ||
          token->type == TOKEN_STRING_TEMPLATE_END ||
          token->type == TOKEN_STRING_TEMPLATE_PART_START ||
          token->type == TOKEN_STRING_TEMPLATE_PART_END) {
        token = token->next;

        if (current_node != NULL) {
          list_head = list_head->next = current_node;
          current_node = NULL;
        }

        continue;
      }

      // this is a parseable token at this point
      current_node = parse_node(token, current_node);
      token = current_node->end;
    }

    // link any remaining nodes
    // current_node could be null, but should be fine
    list_head->next = current_node;

    node->parts = head.next;
    node->end = token = token->next; // skip TOKEN_STRING_TEMPLATE_END

    return node;
  }

  // array
  if (token->type == TOKEN_SYMBOL && starts_with(token->value, "[")) {
    Node *node = create_node(NODE_ARRAY, token, NULL);

    token = token->next; // skip [

    Node head = {0};
    Node *list_head = &head;
    Node *current_node = NULL;
    while (token && (!starts_with(token->value, "]"))) {
      // link to body on ; or ,
      if (current_node != NULL &&
          (starts_with(token->value, ";") || starts_with(token->value, ","))) {
        token = token->next;
        list_head = list_head->next = current_node;
        current_node = NULL;
        continue;
      }

      // this is a parseable token at this point
      current_node = parse_node(token, current_node);
      token = current_node->end;
    }

    // link any remaining nodes
    // current_node could be null, but should be fine
    list_head->next = current_node;

    node->parts = head.next;
    node->end = token = token->next; // skip ]

    return node;
  }

  // block
  if (token->type == TOKEN_SYMBOL && starts_with(token->value, "{")) {
    Node *node = create_node(NODE_BLOCK, token, NULL);

    token = token->next; // skip {

    Node head = {0};
    Node *list_head = &head;
    Node *current_node = NULL;

    while (token && (!starts_with(token->value, "}"))) {
      // link statement to body
      if (current_node != NULL &&
          (starts_with(token->value, ";") || starts_with(token->value, ","))) {
        token = token->next;
        list_head = list_head->next = current_node;
        current_node = NULL;
        continue;
      }

      // this is a parseable token at this point
      current_node = parse_node(token, current_node);
      token = current_node->end;
    }

    // link any remaining nodes
    // current_node could be null, but should be fine
    list_head->next = current_node;

    node->parts = head.next;
    node->end = token = token->next; // skip }

    return node;
  }

  // // identifier keyword: if
  // if (current_token->type == TOKEN_IDENTIFIER &&
  //     starts_with(current_token->value, "if")) {
  //   NodeConditional value;

  //   // conditional
  //   Token sub_head = *current_token->next;
  //   Token *current_sub_token = &sub_head;
  //   while (current_sub_token->next &&
  //          !starts_with(current_sub_token->next->value, "{")) {
  //     current_sub_token = current_sub_token->next;
  //   }
  //   current_token = current_sub_token->next;
  //   current_sub_token->next = NULL;
  //   value.expression = parse(&sub_head, NULL);

  //   // then block
  //   sub_head = *current_token;
  //   current_sub_token = &sub_head;
  //   size_t matching_token_count = 0; // for counting matching tokens
  //   while (current_sub_token) {
  //     current_sub_token = current_sub_token->next;

  //     if (starts_with(current_sub_token->value, "{")) {
  //       matching_token_count = matching_token_count + 1;
  //     } else if (starts_with(current_sub_token->value, "}") &&
  //                matching_token_count > 0) {
  //       matching_token_count = matching_token_count - 1;
  //     } else if (starts_with(current_sub_token->value, "}") &&
  //                matching_token_count == 0) {
  //       break;
  //     }
  //   }
  //   current_token = current_sub_token->next;
  //   current_sub_token->next = NULL;
  //   value.if_then = parse(&sub_head, NULL);

  //   // else block
  //   if (current_token->type == TOKEN_IDENTIFIER &&
  //       starts_with(current_token->value, "else")) {
  //     sub_head = *current_token->next;
  //     current_sub_token = &sub_head;
  //     matching_token_count = 0; // for counting matching tokens
  //     while (current_sub_token &&
  //            (!starts_with(current_sub_token->value, "}") &&
  //             matching_token_count == 0)) {
  //       current_sub_token = current_sub_token->next;

  //       if (starts_with(current_sub_token->value, "{")) {
  //         matching_token_count = matching_token_count + 1;
  //       } else if (starts_with(current_sub_token->value, "}") &&
  //                  matching_token_count > 0) {
  //         matching_token_count = matching_token_count - 1;
  //       }
  //     }
  //     current_token = current_sub_token->next;
  //     current_sub_token->next = NULL;
  //     value.if_else = parse(&sub_head, NULL);
  //   }

  //   last_node = create_node(NODE_STATEMENT_CONDITIONAL, &value);
  //   continue;
  // }

  // // function
  // if (current_token->type == TOKEN_SYMBOL &&
  //     starts_with(current_token->value, "=>") && last_node != NULL &&
  //     last_node->type == NODE_EXPRESSION) {
  //   current_token = (current_token)->next; // skip =
  //   current_token = (current_token)->next; // skip >

  //   Token sub_head = *current_token;
  //   Token *current_sub_token = &sub_head;
  //   size_t matching_token_count = 0; // for counting matching tokens
  //   while (current_sub_token &&
  //          (!starts_with(current_sub_token->value, "}") &&
  //           matching_token_count == 0)) {
  //     current_sub_token = current_sub_token->next;

  //     if (starts_with(current_sub_token->value, "{")) {
  //       matching_token_count = matching_token_count + 1;
  //     } else if (starts_with(current_sub_token->value, "}") &&
  //                matching_token_count > 0) {
  //       matching_token_count = matching_token_count - 1;
  //     };
  //   }
  //   current_token = current_sub_token->next; // skip }
  //   current_sub_token->next = NULL;

  //   Node *block = parse(&sub_head, NULL);
  //   Function *function = malloc(sizeof(*function));
  //   function->parameters = last_node;
  //   function->block = block;
  //   last_node = create_node(NODE_LITERAL_FUNCTION, function);
  //   continue;
  // }

  // expression
  if (token->type == TOKEN_SYMBOL && starts_with(token->value, "(")) {
    Node *node = create_node(NODE_EXPRESSION, token, NULL);

    token = token->next; // skip (

    while (token && !starts_with(token->value, ")")) {
      node->body = parse_node(token, node->body);
      token = node->body->end;
    }

    node->end = token = token->next; // skip )

    return node;
  }

  // expression - assignment
  if (token->type == TOKEN_SYMBOL && starts_with(token->value, ":")) {
    Node *node = create_node(NODE_EXPRESSION_ASSIGNMENT, token, NULL);
    node->variable = last_node;

    token = token->next; // skip :

    while (token && (!starts_with(token->value, ";") &&
                     !starts_with(token->value, ","))) {
      node->value = parse_node(token, node->value);
      token = node->value->end;
    }

    node->end = token;
    return node;
  }

  // expression - binary
  if (token->type == TOKEN_SYMBOL &&
      (starts_with(token->value, "+") || starts_with(token->value, "-") ||
       starts_with(token->value, "/") || starts_with(token->value, "*") ||
       starts_with(token->value, "%%") || starts_with(token->value, "<") ||
       starts_with(token->value, ">") || starts_with(token->value, "<=") ||
       starts_with(token->value, ">="))) {

    Node *binary_root_node = last_node;
    while (token && (!starts_with(token->value, ";") &&
                     !starts_with(token->value, ")") &&
                     !starts_with(token->value, "}"))) {
      char *operator_symbol = get_operator(token);

      if (binary_root_node != NULL &&
          binary_root_node->type == NODE_EXPRESSION_BINARY &&
          get_operator_precedence(binary_root_node->operator_symbol) <
              get_operator_precedence(operator_symbol)) {
        // when we have multiple binary expressions in a row, we
        // need to compare their operator precedence, and if the current
        // binary expression has greater precendence, we have to steal
        // the right operand from the lesser precedence, and replace
        // it with this binary expression
        Node *node = create_node(NODE_EXPRESSION_BINARY, token, NULL);
        node->operator_symbol = operator_symbol;
        node->left = binary_root_node->right;
        binary_root_node->right = node;
        token = token->next;
        node->right = parse_node(token, NULL);
        token = binary_root_node->end = node->end = node->right->end;
      } else {
        Node *node = create_node(NODE_EXPRESSION_BINARY, token, NULL);
        node->operator_symbol = operator_symbol;
        node->left = binary_root_node;
        token = token->next;
        node->right = parse_node(token, NULL);
        token = node->end = node->right->end;
        binary_root_node = node;
      }
    }

    return binary_root_node;
  }

  static const char *token_types[] = {
      "TOKEN_IDENTIFIER",
      "TOKEN_NUMBER",
      "TOKEN_NUMBER_ALTERNATIVE_BASE",
      "TOKEN_STRING",
      "TOKEN_STRING_TEMPLATE_START",
      "TOKEN_STRING_TEMPLATE_END",
      "TOKEN_STRING_TEMPLATE_PART_START",
      "TOKEN_STRING_TEMPLATE_PART_END",
      "TOKEN_SYMBOL",
      "TOKEN_END_OF_FILE",
  };
  printf("Could not parse token: %s, \"%.*s\"\n", token_types[token->type],
         (int)token->length, token->value);
  exit(1);
}

Node *parse(Token *token, Node *parent_node) {
  Node head = {0};
  Node *statements = &head;
  Node *last_node = NULL;

  Token *current_token = token;
  while (current_token && current_token->type != TOKEN_END_OF_FILE) {
    // statement terminator
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, ";")) {
      statements = statements->next = last_node;
      last_node = NULL;
      current_token = current_token->next;
      continue;
    }

    // elision
    if (current_token->type == TOKEN_SYMBOL &&
        starts_with(current_token->value, ",")) {
      statements = statements->next = last_node;
      current_token = current_token->next;
      continue;
    }

    last_node = parse_node(current_token, last_node);
    current_token = last_node->end;
  }

  return head.next;
}
