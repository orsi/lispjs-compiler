#ifndef LEX_H
#define LEX_H

enum TokenType {
  TOKEN_IDENTIFIER,
  TOKEN_KEYWORD,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_SYMBOL,
};
typedef struct Token Token;
struct Token {
  enum TokenType type;
  int length;
  char *value;
  Token *next;
};

int is_keyword(char *word);
int string_starts_with(char *string, char *starts_with);
void print_tokens(Token *token);
Token *lex(char *input);

#endif