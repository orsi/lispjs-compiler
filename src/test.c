#include "../src/lex.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Simple expression: \"1 + 2 + 3\"\n");
  Token *tokens = lex("1 + 2 + 3\0");

  printf("Order of precedence: \"1 + 2 * 10\"\n");
  tokens = lex("1 + 2 * 10\0");

  printf("Multiple order of precedence: \"(2 * (1 + 5)) / -8 - (3 + 3)\"\n");
  tokens = lex("(2 * (1 + 5)) / -8 - (3 + 3)\0");

  printf("Bigger numbers: \"909450945082 + 2392038 - 12390190485\"\n");
  tokens = lex("909450945082 + 2392038 - 12390190485\0");

  printf("Decimal numbers: \".123190024234 + 1231231249.34989283423\"\n");
  tokens = lex(".123190024234 + 1231231249.34989283423\0");

  printf("Unbalanced parentheses should error: \"2 + (5 * 2\"\n");
  tokens = lex("2 + (5 * 2\0");

  printf("Unbalanced parentheses should error: \"(((((4))))\"\n");
  tokens = lex("(((((4))))\0");

  return 0;
}
