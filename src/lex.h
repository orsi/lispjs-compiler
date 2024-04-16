#ifndef LEX_H
#define LEX_H

enum TokenType {
  Identifier,
  Keyword,
  Number,
  String,
  Symbol,
};
typedef struct Token Token;
struct Token {
  enum TokenType type;
  int length;
  char *value;
  Token *next;
};
#endif