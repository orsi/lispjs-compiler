#include "roxanne.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string_duplicate(char *destination, size_t length) {
  if (destination == NULL) {
    destination = calloc(length + 1, sizeof(char));
  } else {
    destination = realloc(destination, length + 1);
  }

  if (destination == NULL) {
    printf("Can't malloc destination\n");
    exit(1);
  }

  return destination;
}

bool starts_with(const char *a, const char *b) {
  return strncmp(a, b, strlen(b)) == 0;
}

char *read_filepath(const char *filepath) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    printf("Error: Filepath %s does not exist.\n", filepath);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);

  char *contents = malloc(file_size + 1);
  if (contents == NULL) {
    printf("No malloc :(\n");
    exit(1);
  }

  rewind(file);
  fread(contents, file_size, 1, file);
  fclose(file);

  // null-terminate
  contents[file_size] = '\0';
  return contents;
}

char *stringify_token(Token *token) {
  char *token_string = NULL;
  int length;
  switch (token->type) {
  case TOKEN_IDENTIFIER:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->start);
    token_string = malloc(sizeof("token:id:") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string\n");
      exit(1);
    }
    sprintf(token_string, "token:id:%.*s", (int)token->length, token->start);
    break;
  case TOKEN_KEYWORD:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->start);
    token_string = malloc(sizeof("token:keyword:") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string\n");
      exit(1);
    }
    sprintf(token_string, "token:keyword:%.*s", (int)token->length,
            token->start);
    break;
  case TOKEN_NUMBER:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->start);
    token_string = malloc(sizeof("token:number:") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string\n");
      exit(1);
    }
    sprintf(token_string, "token:number:%.*s", (int)token->length,
            token->start);
    break;
  case TOKEN_STRING:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->start);
    token_string = malloc(sizeof("token:string:") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string\n");
      exit(1);
    }
    sprintf(token_string, "token:string:%.*s", (int)token->length,
            token->start);
    break;
  case TOKEN_SYMBOL:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->start);
    token_string = malloc(sizeof("token:symbol:") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string\n");
      exit(1);
    }
    sprintf(token_string, "token:symbol:%.*s", (int)token->length,
            token->start);
    break;
  case TOKEN_END_OF_FILE:
    token_string = malloc(sizeof("token:eof"));
    if (token_string == NULL) {
      printf("Error: malloc token_string\n");
      exit(1);
    }
    strcpy(token_string, "token:eof");
    break;
  case TOKEN_STRING_TEMPLATE_START:
    token_string =
        string_duplicate(token_string, sizeof("token:string_template_start"));
    strcpy(token_string, "token:string_template_start");
    break;
  case TOKEN_STRING_TEMPLATE_END:
    token_string =
        string_duplicate(token_string, sizeof("token:string_template_end"));
    strcpy(token_string, "token:string_template_end");
    break;
  case TOKEN_STRING_TEMPLATE_PART_START:
    token_string = string_duplicate(token_string,
                                    sizeof("token:string_template_part_start"));
    strcpy(token_string, "token:string_template_part_start");
    break;
  case TOKEN_STRING_TEMPLATE_PART_END:
    token_string = string_duplicate(token_string,
                                    sizeof("token:string_template_part_end"));
    strcpy(token_string, "token:string_template_part_end");
    break;
  }

  return token_string;
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

char *stringify_result(Result *result) {
  char string[500];
  size_t length = 0;
  switch (result->type) {
  case (RESULT_ARRAY): {
    // length = sprintf(string, "result:array:[");

    // for (size_t i = 0; i < result->array.length; i++) {
    //   Node *part = get_array_item_at(&result->array, i);
    //   Result *part_result = evaluate(part);
    //   if (part_result->type == RESULT_STRING) {
    //     length += part_result->string.length + 2;
    //     strncat(string, "\"", 1);
    //     strncat(string, part_result->string.value,
    //     part_result->string.length); strncat(string, "\"", 1);
    //   } else if (part_result->type == RESULT_BOOLEAN) {
    //     char buffer[128];
    //     size_t l =
    //         sprintf(buffer, "%s", part_result->boolean ? "true" : "false");
    //     length += l;
    //     strncat(string, buffer, l);
    //   } else {
    //     char buffer[128];
    //     size_t l = sprintf(buffer, "%g", part_result->number);
    //     length += l;
    //     strncat(string, buffer, l);
    //   }

    //   if (i < result->array.length - 1) {
    //     strncat(string, ", ", 2);
    //     length += 2;
    //   }
    // }
    // length += 1;
    // strncat(string, "]", 1);
    break;
  }
  case (RESULT_BOOLEAN): {
    length = sprintf(string, "result:boolean:%s",
                     result->boolean ? "true" : "false");
    break;
  }
  case (RESULT_NONE): {
    length = sprintf(string, "result:none");
    break;
  }
  case (RESULT_NUMBER): {
    length = sprintf(string, "result:number:%f", result->number);
    break;
  }
  case RESULT_STRING: {
    length = sprintf(string, "result:string:\"%.*s\"", result->string.length,
                     result->string.value);
    break;
  }
  }

  char *result_string = malloc(length);
  if (result_string == NULL) {
    printf("Error: malloc result_string\n");
    exit(1);
  }
  memcpy(result_string, string, length);
  return result_string;
}

void print_tokens(Token *token) {
  size_t length = 0;
  while (token) {
    printf("%s\n", stringify_token(token));
    token = token->next;
    length += 1;
  }
  printf("total: %zu\n", length);
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

void print_result(Result *result) { printf("%s\n", stringify_result(result)); }
