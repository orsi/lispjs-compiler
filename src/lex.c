#include "roxanne.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *create_token(enum TokenType type, char *string, int length) {
  Token *token = malloc(sizeof(Token));
  if (token == NULL) {
    printf("Error: malloc token");
    exit(1);
  }
  token->type = type;
  token->value = string;
  token->length = length;
  return token;
}

Token *lex(char *string) {
  Token head = {};
  Token *current_token = &head;

  while (string[0]) {
    // comment: single-line
    if (starts_with(string, "//")) {
      while (*string && !starts_with(string, "\n")) {
        string++;
      }
      continue;
    }

    // comment: multi-line
    if (starts_with(string, "/*")) {
      while (*string && !starts_with(string, "*/")) {
        string++;
      }
      string += 2; // go past last "*/"
      continue;
    }

    // spaces/new-lines
    if (isspace(string[0])) {
      while (*string && isspace(string[0])) {
        string++;
      }
      continue;
    }

    // string literal
    if (starts_with(string, "'") || starts_with(string, "\"")) {
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
      current_token = current_token->next = token;
      string += current_token->length + 2; // +2 for advancing past both quotes
      continue;
    }

    // number literal
    if (isdigit(string[0])) {
      char *current = string;
      while (*current &&
                 (isdigit(current[0]) || // is digit
                  (current[0] == '.' &&
                   isdigit(current[1])) || // is point followed by digit
                  (current[0] == ',' &&
                   isdigit(current[1]))) || // is comma followed by digit
             (current[0] == ' ' &&
              isdigit(current[1]))) // is space followed by digit
      {
        current++;
      }

      size_t length = current - string;
      Token *token = create_token(TOKEN_NUMBER, string, length);
      current_token = current_token->next = token;
      string += current_token->length;
      continue;
    }

    // binary/hexadecimal/octal literals
    if (ispunct(string[0]) && string[0] == '#') {
      char *current = string;
      current++; // advance past #
      char baseIndicator = current[0];
      if (baseIndicator != 'b' && baseIndicator != 'h' &&
          baseIndicator != 'o') {
        printf("Error: #%c is not a valid number base", baseIndicator);
        exit(1);
      }
      current++; // advance past base

      while (*current &&
                 (isalnum(current[0]) || // is digit
                  (current[0] == '.' &&
                   isalnum(current[1])) || // is point followed by digit
                  (current[0] == ',' &&
                   isalnum(current[1]))) || // is comma followed by digit
             (current[0] == ' ' &&
              isalnum(current[1]))) // is space followed by digit
      {
        current++;
      }

      size_t length = current - string;
      Token *token =
          create_token(TOKEN_NUMBER_ALTERNATIVE_BASE, string, length);
      current_token = current_token->next = token;
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
      char *current = string;
      while (isalnum(*current)) {
        current++;
      }

      int length = current - string;

      Token *token;
      token = create_token(TOKEN_IDENTIFIER, string, length);

      current_token = current_token->next = token;
      string += current_token->length;
      continue;
    }

    printf("Error: Cannot lex %s\n", string);
    exit(1);
  }

  current_token = current_token->next =
      create_token(TOKEN_END_OF_FILE, string, 0);
  return head.next;
}