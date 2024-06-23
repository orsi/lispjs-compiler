#ifndef LEX_H
#define LEX_H

#include <stdbool.h>
#include <stddef.h>

enum TokenType {
  TOKEN_END_OF_FILE,
  TOKEN_IDENTIFIER,
  TOKEN_KEYWORD,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_STRING_TEMPLATE_START,
  TOKEN_STRING_TEMPLATE_END,
  TOKEN_STRING_TEMPLATE_PART_START,
  TOKEN_STRING_TEMPLATE_PART_END,
  TOKEN_SYMBOL,
};
typedef struct Token Token;
struct Token {
  enum TokenType type;
  size_t length;
  char *start;
  char *end;
  Token *next;
};
char *stringify_token(Token *token);
void print_tokens(Token *token);
bool is_keyword(char *word, int length);
Token *create_token(enum TokenType type, int length, char *start, char *end);
Token *lex(const char *start);
#endif
