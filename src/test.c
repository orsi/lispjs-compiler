#include "../src/lex.c"
#include <stdio.h>

void print_tokens(Token *token) {
  while (token) {
    char *type;
    switch (token->type) {
    case 0:
      type = "id\0";
      break;
    case 1:
      type = "key\0";
      break;
    case 2:
      type = "num\0";
      break;
    case 3:
      type = "op\0";
      break;
    case 4:
      type = "str\0";
      break;
    }
    printf("└ token: %s, %.*s\n", type, token->length, token->value);
    token = token->next;
  }
}

int main(int argc, char *argv[]) {
  printf("Simple expression:\t\t1 + 2 + 3\n");
  print_tokens(lex("1 + 2 + 3"));

  printf("Bigger numbers:\t\t909450945082 + 2392038 - 12390190485\n");
  print_tokens(lex("909450945082 + 2392038 - 12390190485"));

  printf("Decimal numbers:\t\t.123190024234 + 1231231249.34989283423\n");
  print_tokens(lex(".123190024234 + 1231231249.34989283423"));

  printf("Strings literals:\t\t\"quotes\" \'apostrophes\' `ticks!`\n");
  print_tokens(lex("\"quotes\" \'apostrophes\' `ticks!`"));

  printf("No matching quote:\t\t\"quotes \'apostrophes\' `ticks!`\n");
  print_tokens(lex("\"quotes \'apostrophes\' `ticks!`"));

  printf("String with escapes:\t\t\"Escaped \\\"quote\\\"\" + \"in here\"\n");
  print_tokens(lex("\"Escaped \\\"quote\\\"\" + \"in here\""));

  printf("Identifiers:\twhat: 4;\n");
  print_tokens(lex("what: 4;"));

  printf("Lots of identifiers:\twhat do we do here?\n");
  print_tokens(lex("what do we do here?"));

  printf("Keywords:\tif else hi\n");
  print_tokens(lex("if else 123 not a keyword"));

  printf("Garbage:\t$23o@ a@#F: -2@#; \\ i??jawoea\"wef:3\"\n");
  print_tokens(lex("$23o@ a@#F: -2@#; \\ i??jawoea\"wef:3\""));

  return 0;
}
