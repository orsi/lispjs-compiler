#include "roxanne.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Token *create_token(enum TokenType type, char *start, char *end, int length,
                    char *value) {
  Token *token = malloc(sizeof(Token));
  if (token == NULL) {
    printf("Error: malloc token\n");
    exit(1);
  }
  *token = (Token){type, start, end, length, value, NULL};
  return token;
}

Token *lex_token(const char *start) {
  char *current = (char *)start;

  while (*current) {
    // comment: single-line
    if (starts_with(current, "//")) {
      while (*current && !starts_with(current, "\n")) {
        current++;
      }
      continue;
    }

    // comment: multi-line
    if (starts_with(current, "/*")) {
      while (*current && !starts_with(current, "*/")) {
        current++;
      }
      current += 2; // go past last "*/"
      continue;
    }

    // spaces/new-lines
    if (isspace(current[0])) {
      while (*current && isspace(current[0])) {
        current++;
      }
      continue;
    }

    // string literal
    if (starts_with(current, "'") || starts_with(current, "\"")) {
      char quote = *current;
      char *position = current + 1;
      char *end = position;
      Token token_head = {0};
      Token *token_current = &token_head;

      while (*end && *end != quote) {
        if (starts_with(end, "${")) {
          if (token_head.next == NULL) {
            // create marker for beginning of string template
            token_current = token_current->next = create_token(
                TOKEN_STRING_TEMPLATE_START, position, position, 0, position);
          }
          token_current = token_current->next = create_token(
              TOKEN_STRING, position, end, end - position, position);
          position = end;
          end += 2; // ${
          token_current = token_current->next =
              create_token(TOKEN_STRING_TEMPLATE_PART_START, position, end,
                           end - position, position);
          position = end;
          while (*end && !starts_with(end, "}")) {
            Token *token = lex_token(end);

            if (token == NULL) {
              break;
            } else {
              // fast forward to end of parsed tokens
              while (token) {
                token_current = token_current->next = token;
                end = token->end;
                token = token->next;
              }
            }
            end = token_current->end;
          }
          position = end;
          end += 1; // skip }
          token_current = token_current->next =
              create_token(TOKEN_STRING_TEMPLATE_PART_END, position, end,
                           end - position, position);
          position = end;
        } else {
          end++;
        }
      }

      if (token_head.next == NULL) {
        // regular ol' string
        token_current = token_current->next =
            create_token(TOKEN_STRING, position, end + 1, end - position,
                         position); // end + 1 skips final quote
      } else {
        // final string plus end marker for template string
        token_current = token_current->next =
            create_token(TOKEN_STRING, position, end + 1, end - position,
                         position); // end + 1 skips final quote
        token_current = token_current->next = create_token(
            TOKEN_STRING_TEMPLATE_END, end + 1, end + 1, 0, end + 1);
      }

      return token_head.next;
    }

    // number literal
    if (isdigit(current[0]) || (current[0] == '.' && isdigit(current[1]))) {
      char *end = current;
      while (
          *end &&
          (isdigit(end[0]) ||                    // is digit
           (end[0] == '.' && isdigit(end[1])) || // is point followed by digit
           (end[0] == ' ' && isdigit(end[1])) || // is space followed by digit
           (end[0] == '_' && isdigit(end[1])) // is underscore followed by digit
           )) {
        end++;
      }

      size_t length = end - current;
      Token *token = create_token(TOKEN_NUMBER, current, end, length, current);
      return token;
    }

    // binary/hexadecimal/octal literals
    if (ispunct(current[0]) && current[0] == '#') {
      char *end = current;
      end++; // advance past #
      char baseIndicator = end[0];
      if (baseIndicator != 'b' && baseIndicator != 'B' &&
          baseIndicator != 'h' && baseIndicator != 'H' &&
          baseIndicator != 'o' && baseIndicator != 'O') {
        printf("Error: #%c is not a valid number base\n", baseIndicator);
        exit(1);
      }
      end++; // advance past base

      while (
          *end &&
          (isalnum(end[0]) ||                     // is digit
           ((end[0] == '.' && isalnum(end[1])) || // is point followed by digit
            (end[0] == ',' && isalnum(end[1])) || // is comma followed by digit
            (end[0] == ' ' && isalnum(end[1]))    // is space followed by digit
            ))) {
        end++;
      }

      size_t length = end - start;
      Token *token = create_token(TOKEN_NUMBER_ALTERNATIVE_BASE, current, end,
                                  length, current);
      return token;
    }

    // symbols
    if (ispunct(current[0])) {
      Token *token =
          create_token(TOKEN_SYMBOL, current, current + 1, 1, current);
      return token;
    }

    // identifier
    if (isalpha(current[0])) {
      char *end = current;
      while (isalnum(*end)) {
        end++;
      }

      size_t length = end - current;
      Token *token =
          create_token(TOKEN_IDENTIFIER, current, end, length, current);
      return token;
    }

    printf("Error: could not lex '%c'\n", start[0]);
    exit(1);
  }

  return NULL;
}

Token *lex(const char *start) {
  char *position = (char *)start;
  Token head = {0};
  Token *current_token = &head;

  while (*position) {
    Token *token = lex_token(position);

    if (token == NULL) {
      break;
    } else {
      // fast forward to end of parsed tokens
      while (token) {
        current_token = current_token->next = token;
        position = token->end;
        token = token->next;
      }
    }
  }

  current_token = current_token->next =
      create_token(TOKEN_END_OF_FILE, position, position, 0, position);

  return head.next;
}
