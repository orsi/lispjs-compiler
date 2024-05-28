#include "roxanne.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *create_token(enum TokenType type, char *string, int length, char *end) {
  Token *token = malloc(sizeof(Token));
  if (token == NULL) {
    printf("Error: malloc token");
    exit(1);
  }
  token->type = type;
  token->value = string;
  token->length = length;
  token->end = end;
  return token;
}

Token *lex_token(char *start) {
  while (*start) {
    // comment: single-line
    if (starts_with(start, "//")) {
      while (*start && !starts_with(start, "\n")) {
        start++;
      }
      continue;
    }

    // comment: multi-line
    if (starts_with(start, "/*")) {
      while (*start && !starts_with(start, "*/")) {
        start++;
      }
      start += 2; // go past last "*/"
      continue;
    }

    // spaces/new-lines
    if (isspace(start[0])) {
      while (*start && isspace(start[0])) {
        start++;
      }
      continue;
    }

    // string literal
    if (starts_with(start, "'") || starts_with(start, "\"")) {
      char quote = *start;
      char *position = start + 1;
      char *end = position;
      Token token_head = {0};
      Token *token_current = &token_head;

      while (*end && *end != quote) {
        if (starts_with(end, "${")) {
          if (token_head.next == NULL) {
            // create marker for beginning of string template
            token_current = token_current->next =
                create_token(TOKEN_STRING_TEMPLATE, position, 0, position);
          }
          token_current = token_current->next =
              create_token(TOKEN_STRING, position, end - position, end);
          position = end;
          end += 2; // ${
          token_current = token_current->next = create_token(
              TOKEN_STRING_TEMPLATE_PART_START, position, end - position, end);
          position = end;
          while (*end && !starts_with(end, "}")) {
            token_current = token_current->next = lex_token(end);
            end = token_current->end;
          }
          position = end;
          end += 1; // skip }
          token_current = token_current->next = create_token(
              TOKEN_STRING_TEMPLATE_PART_END, position, end - position, end);
          position = end;
        } else {
          end++;
        }
      }

      if (token_head.next == NULL) {
        // regular ol' string
        token_current = token_current->next =
            create_token(TOKEN_STRING, position, end - position,
                         end + 1); // end + 1 skips final quote
      } else {
        // final string plus end marker for template string
        token_current = token_current->next =
            create_token(TOKEN_STRING, position, end - position,
                         end + 1); // end + 1 skips final quote
        token_current = token_current->next =
            create_token(TOKEN_STRING_TEMPLATE_END, end + 1, 0, end + 1);
      }

      return token_head.next;
    }

    // number literal
    if (isdigit(start[0])) {
      char *position = start;
      while (*position &&
             (isdigit(position[0]) || // is digit
              (position[0] == '.' &&
               isdigit(position[1])) || // is point followed by digit
              (position[0] == ',' &&
               isdigit(position[1])) || // is comma followed by digit
              (position[0] == ' ' &&
               isdigit(position[1])))) // is space followed by digit
      {
        position++;
      }

      size_t length = position - start;
      Token *token = create_token(TOKEN_NUMBER, start, length, position);
      return token;
    }

    // binary/hexadecimal/octal literals
    if (ispunct(start[0]) && start[0] == '#') {
      char *position = start;
      position++; // advance past #
      char baseIndicator = position[0];
      if (baseIndicator != 'b' && baseIndicator != 'B' &&
          baseIndicator != 'h' && baseIndicator != 'H' &&
          baseIndicator != 'o' && baseIndicator != 'O') {
        printf("Error: #%c is not a valid number base", baseIndicator);
        exit(1);
      }
      position++; // advance past base

      while (*position &&
             (isalnum(position[0]) || // is digit
              ((position[0] == '.' &&
                isalnum(position[1])) || // is point followed by digit
               (position[0] == ',' &&
                isalnum(position[1])) || // is comma followed by digit
               (position[0] == ' ' &&
                isalnum(position[1])) // is space followed by digit
               ))) {
        position++;
      }

      size_t length = position - start;
      Token *token =
          create_token(TOKEN_NUMBER_ALTERNATIVE_BASE, start, length, position);
      return token;
    }

    // symbols
    if (ispunct(start[0])) {
      Token *token = create_token(TOKEN_SYMBOL, start, 1, start + 1);
      return token;
    }

    // identifier
    if (isalpha(start[0])) {
      char *current = start;
      while (isalnum(*current)) {
        current++;
      }

      size_t length = current - start;
      Token *token =
          create_token(TOKEN_IDENTIFIER, start, length, start + length);
      return token;
    }

    printf("Error: could not lex '%c'\n", start[0]);
    exit(1);
  }

  return create_token(TOKEN_END_OF_FILE, start, 0, start);
}

Token *lex(const char *start) {
  char *position = (char *)start;
  Token head = {0};
  Token *current_token = &head;

  while (*position) {
    Token *token = lex_token(position);
    while (token) {
      current_token = current_token->next = token;
      position = token->end;
      token = token->next;
    }
  }

  current_token = current_token->next =
      create_token(TOKEN_END_OF_FILE, position, 0, position);
  return head.next;
}
