#include "./roxanne.h"
#include <ctype.h>
#include <malloc/_malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *keywords[] = {"if",    "import", "else", "export",
                           "false", "return", "true"};

int is_keyword(char *word, int length) {
  int is_match = 0;
  for (int i = 0; i < sizeof(keywords) / sizeof(*keywords); i++) {
    char *key = keywords[i];
    is_match = strncmp(word, key, length) == 0; // i hate this
    if (is_match)
      break;
  }
  return is_match;
}

Token *create_token(enum TokenType type, char *string, int length) {
  Token *token = malloc(sizeof(Token));
  token->type = type;
  token->value = string;
  token->length = length;
  return token;
}

static Token *get_number_literal(char *string) {
  char *start = string;
  char *current = start;
  while (*current &&
         (isdigit(*current) || (*current == '.' && isdigit(current[1])))) {
    // skips over escape chars
    if (*current == '\\') {
      current++;
    }

    current++;
  }

  int length = current - start;
  Token *token = create_token(TOKEN_NUMBER, start, length);
  return token;
}

static Token *get_string_literal(char *string) {
  char quote = *string;
  char *start = string + 1;
  char *current = start;
  while (*current != quote) {
    // if string ends before quote, error
    if (*current == '\0') {
      printf("error: no matching quote (%c) for: %c%.*s\n", quote, quote,
             (int)(current - start), start);
      exit(1);
    }

    current++;
  }

  int length = current - start;
  Token *token = create_token(TOKEN_STRING, start, length);
  current += 1; // skip past remaining quote
  return token;
}

static Token *get_alpha_or_keyword(char *string) {
  char *start = string;
  char *current = start;
  while (isalnum(*current)) {
    current++;
  }

  int length = current - start;

  Token *token;
  if (is_keyword(start, length)) {
    token = create_token(TOKEN_KEYWORD, start, length);
  } else {
    token = create_token(TOKEN_IDENTIFIER, start, length);
  }

  return token;
}

char *get_token_string(Token *token) {
  char *token_string = NULL;
  int length;
  switch (token->type) {
  case TOKEN_IDENTIFIER:
    length = snprintf(NULL, 0, "%.*s", token->length, token->value);
    token_string = malloc(sizeof("token:id, ") + length);
    sprintf(token_string, "token:id, %.*s", token->length, token->value);
    break;
  case TOKEN_KEYWORD:
    length = snprintf(NULL, 0, "%.*s", token->length, token->value);
    token_string = malloc(sizeof("token:keyword, ") + length);
    sprintf(token_string, "token:keyword, %.*s", token->length, token->value);
    break;
  case TOKEN_NUMBER:
    length = snprintf(NULL, 0, "%.*s", token->length, token->value);
    token_string = malloc(sizeof("token:number, ") + length);
    sprintf(token_string, "token:number, %.*s", token->length, token->value);
    break;
  case TOKEN_STRING:
    length = snprintf(NULL, 0, "%.*s", token->length, token->value);
    token_string = malloc(sizeof("token:string, ") + length);
    sprintf(token_string, "token:string, %.*s", token->length, token->value);
    break;
  case TOKEN_SYMBOL:
    length = snprintf(NULL, 0, "%.*s", token->length, token->value);
    token_string = malloc(sizeof("token:symbol, ") + length);
    sprintf(token_string, "token:symbol, %.*s", token->length, token->value);
    break;
  case TOKEN_END_OF_FILE:
    token_string = malloc(sizeof("token:eof"));
    strcpy(token_string, "token:eof");
    break;
  default:
    length = snprintf(NULL, 0, "%.*s", token->length, token->value);
    token_string = malloc(sizeof("token:unknown, ") + length);
    sprintf(token_string, "token:unknown, %.*s", token->length, token->value);
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

Token *lex(char *string) {
  Token head = {};
  Token *current_token = &head;

  while (string[0]) {
    // single-line comments
    if (str_starts_with(string, "//")) {
      while (*string && !str_starts_with(string, "\n")) {
        string++;
      }
      continue;
    }

    // multi-line comments
    if (str_starts_with(string, "/*")) {
      while (*string && !str_starts_with(string, "*/")) {
        string++;
      }
      string += 2; // go past last "*/"
      continue;
    }

    // skip spaces/new-lines
    if (isspace(string[0])) {
      while (*string && isspace(string[0])) {
        string++;
      }
      continue;
    }

    // string literals
    if (str_starts_with(string, "'") || str_starts_with(string, "\"")) {
      current_token = current_token->next = get_string_literal(string);
      string += current_token->length + 2; // +2 for advancing past both quote
      continue;
    }

    // number
    if (isdigit(string[0]) || (string[0] == '.' && isdigit(string[1]))) {
      current_token = current_token->next = get_number_literal(string);
      string += current_token->length;
      continue;
    }

    // symbols
    if (ispunct(string[0])) {
      current_token = current_token->next =
          create_token(TOKEN_SYMBOL, string, 1);
      string++;
      continue;
    }

    // identifier
    if (isalpha(string[0])) {
      current_token = current_token->next = get_alpha_or_keyword(string);
      string += current_token->length;
      continue;
    }

    printf("Error at %s\n", string);
    exit(1);
  }

  current_token = current_token->next =
      create_token(TOKEN_END_OF_FILE, string, 0);
  return head.next;
}