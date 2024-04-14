#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum TokenType { Identifier, Keyword, Number, Operator, String };
typedef struct Token Token;
struct Token {
  enum TokenType type;
  int length;
  char *value;
  Token *next;
};

static char *keywords[] = {"if", "else"};
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

Token *lex(char *input) {
  Token *head = malloc(sizeof(Token));
  Token *current = head;

  while (*input) {
    // skip
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
        if (*input == '\\') {
          input++;
        } else if (*input == '\0') {
          printf("error: no matching quote (%c) for: %c%.*s\n", matchingQuote,
                 matchingQuote, (int)(input - start), start);
          is_error = 1;
          break;
        }
        input++;
      }

      if (is_error) {
        continue;
      } else {
        int length = input - start;
        char *value = malloc(length * sizeof(char));
        strncpy(value, start, length);

        Token *t = malloc(sizeof(Token));
        t->type = String;
        t->value = value;
        t->length = length;
        current = current->next = t;
        input++;
      }
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
      t->type = Number;
      t->value = value;
      t->length = length;
      current = current->next = t;
      continue;
    }

    // symbols
    if (ispunct(*input)) {
      Token *t = malloc(sizeof(Token));
      t->type = Operator;
      t->value = input;
      t->length = 1;
      current = current->next = t;
      input++;
      continue;
    }

    // identifiers
    if (isalpha(*input)) {
      char *start = input;
      while (isalpha(*input)) {
        input++;
      }

      int length = input - start;
      char *value = malloc(length * sizeof(char));
      strncpy(value, start, length);

      Token *t = malloc(sizeof(Token));
      if (is_keyword(value)) {
        t->type = Keyword;
        t->value = value;
        t->length = length;
        current = current->next = t;
      } else {
        t->type = Identifier;
        t->value = value;
        t->length = length;
        current = current->next = t;
      }

      continue;
    }

    printf("error: %c\n", *input);
    exit(1);
  }

  return head->next;
}