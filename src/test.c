#include "../src/lex.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Simple expression: \"1 + 2 + 3\"\n");
  Token *tokens = lex("1 + 2 + 3");

  printf("Order of precedence: \"1 + 2 * 10\"\n");
  tokens = lex("1 + 2 * 10");

  printf("Multiple order of precedence: \"(2 * (1 + 5)) / -8 - (3 + 3)\"\n");
  tokens = lex("(2 * (1 + 5)) / -8 - (3 + 3)");

  printf("Unbalanced parentheses should error: \"2 + (5 * 2\"\n");
  tokens = lex("2 + (5 * 2");

  printf("Unbalanced parentheses should error: \"(((((4))))\"\n");
  tokens = lex("(((((4))))");

  return 0;
}
