enum TokenType { Symbol, Literal, Keyword };
typedef struct {
  enum TokenType type;
  char *value;
  int column;
  int line;
} Token;

Token *lex(char *input) {
  Token *tokens = {0};
  return tokens;
}