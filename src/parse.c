#include "parse.h"
#include "evaluate.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_operator(Token *token) {
  Token *current_token = token;
  size_t length = 0;

  while (current_token && length < current_token->length &&
         ispunct(current_token->start[0])) {
    length += 1;
    current_token = token->next;
  }

  char *operator_symbol = malloc(sizeof(char) * length);
  strncpy(operator_symbol, token->start, length);
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

char *stringify_list(Node *node) {
  size_t list_string_length = 0;
  char *list_string = {0};
  Node *current_node = node;

  while (current_node) {
    char *item_string = stringify_node_value(current_node);
    size_t item_string_length = strlen(item_string);
    list_string_length += item_string_length;
    list_string = string_duplicate(list_string, list_string_length);
    strcat(list_string, item_string);

    if (current_node->next != NULL) {
      list_string_length += 2;
      list_string = string_duplicate(list_string, list_string_length);
      strcat(list_string, ", ");
    }

    current_node = current_node->next;
  }

  return list_string;
}

char *stringify_node_value(Node *node) {
  size_t length = 0;
  char *destination = malloc(0);
  strcpy(destination, "");

  if (node->type == NODE_LITERAL_IDENTIFIER) {
    size_t identifier_length = strlen(node->identifier);
    destination = string_duplicate(destination, identifier_length + 1);
    strcat(destination, node->identifier);
  } else if (node->type == NODE_LITERAL_BOOLEAN) {
    destination = string_duplicate(destination, node->boolean ? 4 : 5);
    strcat(destination, node->boolean ? "true" : "false");
  } else if (node->type == NODE_LITERAL_NUMBER) {
    char buf[128];
    size_t number_length;
    if (node->base != 10) {
      number_length = sprintf(buf, "%.16g(%.*s)", node->number,
                              (int)node->start->length, node->start->start);
    } else {
      number_length = sprintf(buf, "%.16g", node->number);
    }
    destination = string_duplicate(destination, number_length);
    strncat(destination, buf, number_length);
  } else if (node->type == NODE_ARRAY) {
    char *items_string = stringify_list(node->body);
    length += strlen(items_string) + 2;
    destination = string_duplicate(destination, length);
    strcat(destination, "[");
    strcat(destination, items_string);
    strcat(destination, "]");
  } else if (node->type == NODE_BLOCK) {
    strcat(destination, "{");
    if (node->body != NULL) {
      char *items_string = stringify_list(node->body);
      length += strlen(items_string) + 2;
      destination = string_duplicate(destination, length);
      strcat(destination, items_string);
    }
    strcat(destination, "}");
  } else if (node->type == NODE_FUNCTION) {
    length += 1;
    destination = string_duplicate(destination, length);
    strcat(destination, "|");

    // parameters
    if (node->parameters != NULL) {
      char *items_string = stringify_list(node->parameters);
      length += strlen(items_string);
      destination = string_duplicate(destination, length);
      strcat(destination, items_string);
    }

    char *block_string = stringify_node_value(node->block);
    length += strlen(block_string);
    destination = string_duplicate(destination, length);
    strcat(destination, block_string);
  } else if (node->type == NODE_LITERAL_STRING) {
    destination = string_duplicate(destination, node->string->length + 2);
    strncat(destination, "\"", 1);
    strncat(destination, node->string->value, node->string->length);
    strncat(destination, "\"", 1);
  } else if (node->type == NODE_LITERAL_STRING_TEMPLATE) {
    Result *result = evaluate(node);
    destination = string_duplicate(destination, result->string.length + 2);
    strncat(destination, "\"", 1);
    strncat(destination, result->string.value, result->string.length);
    strncat(destination, "\"", 1);
  } else if (node->type == NODE_EXPRESSION) {
    char *body_string = stringify_node_value(node->body);
    length += strlen(body_string) + 2;
    destination = string_duplicate(destination, length);
    strcat(destination, "(");
    strcat(destination, body_string);
    strcat(destination, ")");
  } else if (node->type == NODE_EXPRESSION_ASSIGNMENT) {
    size_t identifier_length = strlen(node->variable->identifier);
    char *value_string = stringify_node_value(node->value);
    size_t value_length = strlen(value_string);
    destination =
        string_duplicate(destination, identifier_length + value_length + 1);
    strcat(destination, node->variable->identifier);
    strncat(destination, ":", 1);
    strcat(destination, value_string);
  } else if (node->type == NODE_EXPRESSION_BINARY) {
    size_t operator_length = strlen(node->operator_symbol);
    char *left_value_string = {0};
    size_t left_value_length = 0;
    char *right_value_string = {0};
    size_t right_value_length = 0;

    if (node->left != NULL) {
      left_value_string = stringify_node_value(node->left);
      left_value_length = strlen(left_value_string);
    }

    if (node->right != NULL) {
      right_value_string = stringify_node_value(node->right);
      right_value_length = strlen(right_value_string);
    }

    destination =
        string_duplicate(destination, left_value_length + operator_length +
                                          right_value_length + 1);

    if (left_value_string != NULL) {
      strcat(destination, left_value_string);
    }
    strcat(destination, node->operator_symbol);
    if (right_value_string != NULL) {
      strcat(destination, right_value_string);
    }
  } else {
    char buf[128];
    size_t number_length = sprintf(buf, "%.16g", node->number);
    destination = string_duplicate(destination, number_length);
    strncat(destination, buf, number_length);
  }

  return destination;
}

char *stringify_node(Node *node) {
  char buffer[500];
  int length;

  if (node == NULL) {
    return NULL;
  }

  switch (node->type) {
  case NODE_EXPRESSION:
    length = sprintf(buffer, "node:expression:%s", stringify_node_value(node));
    break;
  case (NODE_EXPRESSION_BINARY):
    length = sprintf(buffer, "node:binary:%s", stringify_node_value(node));
    break;
  case (NODE_EXPRESSION_ASSIGNMENT):
    length = sprintf(buffer, "node:assignment:%s", stringify_node_value(node));
    break;
  case NODE_LITERAL_BOOLEAN:
    length = sprintf(buffer, "node:boolean:%s", stringify_node_value(node));
    break;
  case NODE_ARRAY: {
    length = sprintf(buffer, "node:array:%s", stringify_node_value(node));
    break;
  }
  case (NODE_LITERAL_IDENTIFIER):
    length = sprintf(buffer, "node:identifier:%s", stringify_node_value(node));
    break;
  case (NODE_FUNCTION):
    length = sprintf(buffer, "node:function:%s", stringify_node_value(node));
    break;
  case (NODE_LITERAL_NUMBER):
    length = sprintf(buffer, "node:number:%s", stringify_node_value(node));
    break;
  case (NODE_LITERAL_STRING):
    length = sprintf(buffer, "node:string:%s", stringify_node_value(node));
    break;
  case NODE_LITERAL_STRING_TEMPLATE:
    length =
        sprintf(buffer, "node:string template:%s", stringify_node_value(node));
    break;
  case NODE_CONDITIONAL:
    length = sprintf(buffer, "node:conditional:%s", stringify_node_value(node));
    break;
  case NODE_BLOCK:
    length = sprintf(buffer, "node:block:%s", stringify_node_value(node));
    break;
  case NODE_PROGRAM:
    length = sprintf(buffer, "node:program");
    break;
  }
  char *node_string = {0};
  node_string = string_duplicate(node_string, length);
  strcpy(node_string, buffer);
  return node_string;
}

void print_nodes(Node *node, int level, const char *prefix) {
  int indent = level * 2;
  printf("%*s└ %s%s\n", indent, "", prefix, stringify_node(node));
}

void print_program(Node *node_program) {
  Node *current = node_program->body;
  while (current) {
    print_nodes(current, 0, " ");
    current = current->next;
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
    Node *node = create_node(NODE_LITERAL_NUMBER, token, NULL);
    char normalized_number[token->length + 1];
    char *current_char = token->start;

    // if number starts with #, it is an alternative base, else parse as
    // base 10 by default
    if (starts_with(token->start, "#")) {
      current_char++; // # skip
      current_char++; // b/h/o skip
      node->base = current_char[0] == 'b' ? 2 : current_char[0] == 'o' ? 8 : 16;
    } else {
      node->base = 10;
    }

    size_t normalized_length = 0;
    for (size_t i = 0; i < token->length; i++) {
      if (!starts_with(current_char, " ") && !starts_with(current_char, "_")) {
        normalized_number[normalized_length] = current_char[0];
        normalized_length++;
      }
      current_char++;
    }

    normalized_number[normalized_length] = '\0';
    node->number = strtol(normalized_number, NULL, node->base);
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
    node->string->value = token->start;
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
  if (token->type == TOKEN_SYMBOL && starts_with(token->start, "[")) {
    Node *node = create_node(NODE_ARRAY, token, NULL);

    token = token->next; // skip [

    Node head = {0};
    Node *list_head = &head;
    Node *current_node = NULL;
    while (token && !starts_with(token->start, "]")) {
      // link to body on ; or ,
      if (current_node != NULL &&
          (starts_with(token->start, ";") || starts_with(token->start, ","))) {
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
  if (token->type == TOKEN_SYMBOL && starts_with(token->start, "{")) {
    Node *node = create_node(NODE_BLOCK, token, NULL);

    token = token->next; // skip {

    Node head = {0};
    Node *list_head = &head;
    Node *current_node = NULL;

    while (token && !starts_with(token->start, "}")) {
      // link statement to body
      if (current_node != NULL &&
          (starts_with(token->start, ";") || starts_with(token->start, ","))) {
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

  // function
  if (token->type == TOKEN_SYMBOL && starts_with(token->start, "|")) {
    Node *node = create_node(NODE_FUNCTION, token, NULL);

    token = token->next; // skip |

    // parse parameters until block start
    Node head = {0};
    Node *list_head = &head;
    Node *current_node = NULL;

    while (token && !starts_with(token->start, "{")) {
      // link statement to body
      if (current_node != NULL &&
          (starts_with(token->start, ";") || starts_with(token->start, ","))) {
        token = token->next;
        list_head = list_head->next = current_node;
        current_node = NULL;
        continue;
      }

      // TODO: function params
      // parsing the parameters breaks when we have an assignment expression
      // that doesn't end in ; or ,
      // e.g.
      // |a: "string" {}

      // this is a parseable token at this point
      current_node = parse_node(token, current_node);
      token = current_node->end;
    }

    // link any remaining nodes
    // current_node could be null, but should be fine
    list_head->next = current_node;

    node->parameters = head.next;
    node->block = parse_node(token, NULL);

    node->parts = head.next;
    node->end = token = node->block->end; // skip past end function }

    return node;
  }

  // keywords
  if (token->type == TOKEN_KEYWORD) {
    Node *node = create_node(NODE_LITERAL_BOOLEAN, token, NULL);
    if (starts_with(token->start, "true")) {
      node->type = NODE_LITERAL_BOOLEAN;
      node->boolean = true;
    } else if (starts_with(token->start, "false")) {
      node->type = NODE_LITERAL_BOOLEAN;
      node->boolean = false;
    } else {
      printf("Keyword not implemented: %.*s", (int)token->length, token->start);
      exit(1);
    }

    // // identifier keyword: if
    // if (current_token->type == TOKEN_IDENTIFIER &&
    //     starts_with(current_token->start, "if")) {
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

    //     if (starts_with(current_sub_token->start, "{")) {
    //       matching_token_count = matching_token_count + 1;
    //     } else if (starts_with(current_sub_token->start, "}") &&
    //                matching_token_count > 0) {
    //       matching_token_count = matching_token_count - 1;
    //     } else if (starts_with(current_sub_token->start, "}") &&
    //                matching_token_count == 0) {
    //       break;
    //     }
    //   }
    //   current_token = current_sub_token->next;
    //   current_sub_token->next = NULL;
    //   value.if_then = parse(&sub_head, NULL);

    //   // else block
    //   if (current_token->type == TOKEN_IDENTIFIER &&
    //       starts_with(current_token->start, "else")) {
    //     sub_head = *current_token->next;
    //     current_sub_token = &sub_head;
    //     matching_token_count = 0; // for counting matching tokens
    //     while (current_sub_token &&
    //            (!starts_with(current_sub_token->start, "}") &&
    //             matching_token_count == 0)) {
    //       current_sub_token = current_sub_token->next;

    //       if (starts_with(current_sub_token->start, "{")) {
    //         matching_token_count = matching_token_count + 1;
    //       } else if (starts_with(current_sub_token->start, "}") &&
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
    node->identifier = strncpy(node->identifier, token->start, token->length);
    node->identifier[size] = '\0';
    return node;
  }

  // expression
  if (token->type == TOKEN_SYMBOL && starts_with(token->start, "(")) {
    Node *node = create_node(NODE_EXPRESSION, token, NULL);

    token = token->next; // skip (

    while (token && !starts_with(token->start, ")")) {
      node->body = parse_node(token, node->body);
      token = node->body->end;
    }

    node->end = token = token->next; // skip )

    return node;
  }

  // expression - assignment
  if (token->type == TOKEN_SYMBOL && starts_with(token->start, ":") &&
      last_node != NULL && last_node->type == NODE_LITERAL_IDENTIFIER) {
    Node *node = create_node(NODE_EXPRESSION_ASSIGNMENT, token, NULL);
    node->variable = last_node;

    token = token->next; // skip :

    while (token && (!starts_with(token->start, ";") &&
                     !starts_with(token->start, ","))) {

      // TODO: function params
      // parsing the parameters breaks when we have an assignment expression
      // that doesn't end in ; or ,
      // e.g.
      // |a: "string" {}

      node->value = parse_node(token, node->value);
      token = node->value->end;
    }

    node->end = token;
    return node;
  }

  // expression - binary
  if (token->type == TOKEN_SYMBOL &&
      (starts_with(token->start, "+") || starts_with(token->start, "-") ||
       starts_with(token->start, "/") || starts_with(token->start, "*") ||
       starts_with(token->start, "%%") || starts_with(token->start, "<") ||
       starts_with(token->start, ">") || starts_with(token->start, "<=") ||
       starts_with(token->start, ">="))) {

    Node *binary_root_node = last_node;
    while (token && (!starts_with(token->start, ";") &&
                     !starts_with(token->start, ")") &&
                     !starts_with(token->start, "}"))) {
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
      "TOKEN_END_OF_FILE",
      "TOKEN_IDENTIFIER",
      "TOKEN_KEYWORD",
      "TOKEN_NUMBER",
      "TOKEN_STRING",
      "TOKEN_STRING_TEMPLATE_START",
      "TOKEN_STRING_TEMPLATE_END",
      "TOKEN_STRING_TEMPLATE_PART_START",
      "TOKEN_STRING_TEMPLATE_PART_END",
      "TOKEN_SYMBOL",
  };
  printf("Could not parse token: %s, \"%.*s\"\n", token_types[token->type],
         (int)token->length, token->start);
  exit(1);
}

Node *parse(Token *token, Node *parent_node) {
  Node head = {0};
  Node *statements = &head;
  Node *last_node = NULL;

  Token *current_token = token;
  while (current_token) {
    // statement terminator
    if (starts_with(current_token->start, ";") ||
        starts_with(current_token->start, ",") ||
        current_token->type == TOKEN_END_OF_FILE) {
      statements = statements->next = last_node;
      last_node = NULL;
      current_token = current_token->next;
      continue;
    }

    last_node = parse_node(current_token, last_node);
    current_token = last_node->end;
  }

  return head.next;
}
