#include "roxanne.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Array *create_array(void) {
  Array *array = malloc(sizeof(Array));
  if (array == NULL) {
    printf("Could not malloc array.");
    exit(1);
  }
  array->capacity = 5;
  array->length = 0;
  array->items = malloc(sizeof(void *) * array->capacity);
  if (array->items == NULL) {
    printf("Could not malloc array->items.");
    exit(1);
  }
  return array;
}

void *get_array_item_at(Array *array, size_t index) {
  if (index >= array->length) {
    return NULL;
  }

  return array->items[index];
}

Array *push_array(Array *array, void *item) {
  if (array->length == array->capacity) {
    array->capacity *= 2;
    array->items = realloc(array->items, sizeof(void *) * array->capacity);
    if (array->items == NULL) {
      printf("Could not realloc array->items");
      exit(1);
    }
  }

  array->items[array->length] = item;
  array->length += 1;
  return array;
}

void *pop_array(Array *array) {
  if (array->length == 0) {
    return NULL;
  }

  array->length -= 1;
  return get_array_item_at(array, array->length);
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

char *get_token_string(Token *token) {
  char *token_string = NULL;
  int length;
  switch (token->type) {
  case TOKEN_STRING_TEMPLATE:
  case TOKEN_STRING_TEMPLATE_END:
  case TOKEN_STRING_TEMPLATE_PART_START:
  case TOKEN_STRING_TEMPLATE_PART_END:
    break;
  case TOKEN_IDENTIFIER:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->value);
    token_string = malloc(sizeof("token:id, ") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string");
      exit(1);
    }
    sprintf(token_string, "token:id, %.*s", (int)token->length, token->value);
    break;
  case TOKEN_NUMBER:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->value);
    token_string = malloc(sizeof("token:number, ") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string");
      exit(1);
    }
    sprintf(token_string, "token:number, %.*s", (int)token->length,
            token->value);
    break;
  case TOKEN_NUMBER_ALTERNATIVE_BASE:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->value);
    token_string = malloc(sizeof("token:number_alternative_base, ") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string");
      exit(1);
    }
    sprintf(token_string, "token:number, %.*s", (int)token->length,
            token->value);
    break;
  case TOKEN_STRING:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->value);
    token_string = malloc(sizeof("token:string, ") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string");
      exit(1);
    }
    sprintf(token_string, "token:string, %.*s", (int)token->length,
            token->value);
    break;
  case TOKEN_SYMBOL:
    length = snprintf(NULL, 0, "%.*s", (int)token->length, token->value);
    token_string = malloc(sizeof("token:symbol, ") + length);
    if (token_string == NULL) {
      printf("Error: malloc token_string");
      exit(1);
    }
    sprintf(token_string, "token:symbol, %.*s", (int)token->length,
            token->value);
    break;
  case TOKEN_END_OF_FILE:
    token_string = malloc(sizeof("token:eof"));
    if (token_string == NULL) {
      printf("Error: malloc token_string");
      exit(1);
    }
    strcpy(token_string, "token:eof");
    break;
  }

  return token_string;
}

void print_tokens(Token *token) {
  while (token) {
    printf("%s\n", get_token_string(token));
    token = token->next;
  }
}

char *get_node_string(Node *node) {
  char buffer[128];
  int length;

  if (node == NULL) {
    return NULL;
  }

  switch (node->type) {
  case NODE_EXPRESSION:
    length = sprintf(buffer, "node:expression, %s",
                     get_node_string(node->expression));
    break;
  case (NODE_EXPRESSION_BINARY):
    length = sprintf(buffer, "node:binary, %s", node->operator_symbol);
    break;
  case (NODE_EXPRESSION_ASSIGNMENT):
    length = sprintf(buffer, "node:assignment, %s", node->operator_symbol);
    break;
  case NODE_LITERAL_ARRAY:
    length = sprintf(buffer, "node:array, length %zu", node->array->length);
    break;
  case NODE_LITERAL_BOOLEAN:
    length =
        sprintf(buffer, "node:boolean:%s ", node->boolean ? "true" : "false");
    break;
  case NODE_LITERAL_OBJECT:
    length = sprintf(buffer, "node:object");
    break;
  case (NODE_LITERAL_IDENTIFIER):
    length = sprintf(buffer, "node:identifier, %s", node->identifier);
    break;
  case (NODE_LITERAL_NUMBER):
    length = sprintf(buffer, "node:number, %.16g", node->number);
    break;
  case (NODE_LITERAL_STRING):
    length = sprintf(buffer, "node:string, %.*s", node->string->length,
                     node->string->value);
    break;
  case NODE_LITERAL_STRING_TEMPLATE:
    length = sprintf(buffer, "node:string template, %s",
                     get_node_string(get_array_item_at(
                         node->string_template_parts,
                         node->string_template_parts->length - 1)));
    break;
  case NODE_STATEMENT_CONDITIONAL:
    length = sprintf(buffer, "node:conditional, %s",
                     get_node_string(node->expression));
    break;
  case NODE_STATEMENT_BLOCK:
    length =
        sprintf(buffer, "node:block, %s", get_node_string(node->expression));
    break;
  }

  char *node_string = malloc(length + 1);
  if (node_string == NULL) {
    printf("Error: cannot malloc node_string");
    exit(1);
  }
  strcpy(node_string, buffer);
  return node_string;
}

void print_node_tree(Node *node, int level, const char *prefix) {
  int indent = level * 2;
  printf("%*s└ %s%s\n", indent, "", prefix, get_node_string(node));

  if (node->left != NULL) {
    print_node_tree(node->left, level + 1, "l: ");
  }

  if (node->right != NULL) {
    print_node_tree(node->right, level + 1, "r: ");
  }
}

char *get_result_string(Result *result) {
  char string[128];
  int length;
  switch (result->type) {
  case (RESULT_NONE): {
    length = sprintf(string, "result:none");
    break;
  }
  case (RESULT_NUMBER): {
    length = sprintf(string, "result:double, %f", result->number);
    break;
  }
  case RESULT_STRING: {
    length = sprintf(string, "result:string, %.*s", result->string.length,
                     result->string.value);
    break;
  }
  }

  char *result_string = malloc(length);
  if (result_string == NULL) {
    printf("Error: malloc result_string");
    exit(1);
  }
  memcpy(result_string, string, length);
  return result_string;
}

void print_result(Result *result) { printf("%s", get_result_string(result)); }

void print_program(Program *program) {
  for (size_t i = 0; i < program->statements->length; i++) {
    Node *node = get_array_item_at(program->statements, i);
    print_node_tree(node, 0, " ");
  }
}
