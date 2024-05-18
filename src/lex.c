#include "./roxanne.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *keywords[] = {"if", "else"};

char *get_token_string(Token *token) {
  char string[128];
  int length;
  switch (token->type) {
  case TOKEN_NULL:
    length = sprintf(string, "null");
    break;
  case TOKEN_IDENTIFIER:
    length = sprintf(string, "id: %.*s", token->length, token->value);
    break;
  case TOKEN_KEYWORD:
    length = sprintf(string, "key: %.*s", token->length, token->value);
    break;
  case TOKEN_NUMBER:
    length = sprintf(string, "num: %.*s", token->length, token->value);
    break;
  case TOKEN_STRING:
    length = sprintf(string, "str: %.*s", token->length, token->value);
    break;
  case TOKEN_SYMBOL:
    length = sprintf(string, "sym: %.*s", token->length, token->value);
    break;
  }
  char *token_string = malloc(length);
  memcpy(token_string, string, length);
  return token_string;
}

void print_tokens(Token *token) {
  while (token) {
    printf("└ %s\n", get_token_string(token));
    token = token->next;
  }
}

int is_keyword(char *word) {
  int is_match = 0;
  for (int i = 0; i < sizeof(keywords) / sizeof(*keywords); i++) {
    char *key = keywords[i];
    is_match = strcmp(word, key) == 0; // i hate this
    if (is_match)
      break;
  }
  return is_match;
}

int string_starts_with(char *string, char *starts_with) {
  return strncmp(string, starts_with, strlen(starts_with)) == 0;
}

Token *lex(char *input) {
  Token *head = malloc(sizeof(Token));
  Token *current = head;

  while (*input) {
    // single-line comments
    if (string_starts_with(input, "//")) {
      while (*input != '\n' && *input != '\0') {
        input++;
      }
      continue;
    }

    // multi-line comments
    if (string_starts_with(input, "/*")) {
      while (!string_starts_with(input, "*/") && *input != '\0') {
        input++;
      }
      input += 2; // go past */
      continue;
    }

    // skip spaces/new-lines
    if (isspace(*input)) {
      input++;
      continue;
    }

    // string literals
    if (*input == '"' || *input == '`' || *input == '\'') {
      char matchingQuote = *input;
      char *start = ++input;
      int is_error = 0;
      while (*input != matchingQuote) {
        // pass over escape char
        if (*input == '\0') {
          printf("error: no matching quote (%c) for: %c%.*s\n", matchingQuote,
                 matchingQuote, (int)(input - start), start);
          is_error = 1;
          break;
        }

        // skips over escape chars
        if (*input == '\\') {
          input++;
        }

        input++;
      }

      if (is_error) {
        continue;
      }

      int length = input - start;
      char *value = malloc(length * sizeof(char));
      strncpy(value, start, length);

      Token *t = malloc(sizeof(Token));
      t->type = TOKEN_STRING;
      t->value = value;
      t->length = length;
      current = current->next = t;
      input++;
      continue;
    }

    // number
    if (isdigit(*input) || (*input == '.' && isdigit(input[1]))) {
      char *start = input;
      while (((*input >= '0' && *input <= '9') ||
              (*input == '.' && (input[1] >= '0' && input[1] <= '9')))) {
        input++;
      }
      int length = input - start;
      char *value = malloc(length * sizeof(char));
      strncpy(value, start, length);

      Token *t = malloc(sizeof(Token));
      t->type = TOKEN_NUMBER;
      t->value = value;
      t->length = length;
      current = current->next = t;
      continue;
    }

    // symbols
    if (ispunct(*input)) {
      Token *t = malloc(sizeof(Token));
      t->type = TOKEN_SYMBOL;
      t->value = input;
      t->length = 1;
      current = current->next = t;
      input++;
      continue;
    }

    // identifiers
    if (isalpha(*input)) {
      char *start = input;
      while (isalnum(*input)) {
        input++;
      }

      int length = input - start;
      char *value = malloc(length * sizeof(char));
      strncpy(value, start, length);

      Token *t = malloc(sizeof(Token));
      if (is_keyword(value)) {
        t->type = TOKEN_KEYWORD;
        t->value = value;
        t->length = length;
        current = current->next = t;
      } else {
        t->type = TOKEN_IDENTIFIER;
        t->value = value;
        t->length = length;
        current = current->next = t;
      }

      continue;
    }

    printf("error: %c\n", *input);
    exit(1);
  }

  return head->next == NULL ? head : head->next;
}