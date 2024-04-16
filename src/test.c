#include "./lex.c"
#include "./parse.c"
#include <assert.h>
#include <stdio.h>

#define DEBUG_PRINT 1

#define expect(message, expression, error_message)                             \
  TESTS_TOTAL++;                                                               \
  if (expression) {                                                            \
    TESTS_PASSED++;                                                            \
    printf("\x1b[32m✔ Passed:\x1b[0m  %.*s\n", (int)strlen(message), message); \
  } else {                                                                     \
    TESTS_FAILED++;                                                            \
    printf("\x1b[31m✖ Failed:\x1b[0m  %.*s\n", (int)strlen(message), message); \
    if (strlen(error_message) > 0) {                                           \
      printf("\t%.*s\n", (int)strlen(error_message), error_message);           \
    }                                                                          \
  }                                                                            \
  print_nodes(nodes, 11);

static int TESTS_TOTAL = 0;
static int TESTS_PASSED = 0;
static int TESTS_FAILED = 0;

void print_tokens(Token *token) {
#if DEBUG_PRINT == 1
  while (token) {
    char *type;
    switch (token->type) {
    case Identifier:
      type = "id\0";
      break;
    case Keyword:
      type = "key\0";
      break;
    case Number:
      type = "num\0";
      break;
    case String:
      type = "str\0";
      break;
    case Symbol:
      type = "sym\0";
      break;
    }
    printf("└ token: %s, %.*s\n", type, token->length, token->value);
    token = token->next;
  }
#endif
}

void print_nodes(Node *nodes, int length) {
#if DEBUG_PRINT == 1
  for (int i = 0; i < length; i++) {
    Node node = nodes[i];
    char *type;
    switch (node.type) {
    case BinaryExpression:
      type = "bin\0";
      printf("└ node: %s, %c\n", type, node.symbol_value);
      break;
    case DoubleNumber:
      type = "dbl\0";
      printf("└ node: %s, %f\n", type, node.double_value);
      break;
    case IntNumber:
      type = "int\0";
      printf("└ node: %s, %d\n", type, node.int_value);
      break;
    case Root:
      type = "root\0";
      printf("└ node: root\n");
      break;
    }
  }
#endif
}

int main(int argc, char *argv[]) {
  // Token *tokens = lex("// don't parse me!\n// or me!");
  // expect("Comments //", tokens == NULL, "So what.");

  // tokens = lex("/* don't parse me!\nor me!*/");
  // expect("Comments: /* */", tokens == NULL, "Ya I failed.");

  // tokens = lex("/* don't parse me!\nor me!*/\nhi: 123.123 /* yup */");
  // expect("Comments with variables afterwards", tokens != NULL, "");

  // tokens = lex("1 + 2 + 3");
  // expect("Simple expression like 1+2+3", tokens[0].type == Number, "");

  // tokens =
  //     lex("909450945082 + .123190024234 - 1231231249.34989283423 +
  //     00123.123");
  // expect("Numbers, decimals", 1, "");

  // tokens = lex("\"quotes\" \'apostrophes\' `ticks!`");
  // expect("Quotes and string literals (`, ', \")", 1, "");

  // tokens = lex("\"quotes \'apostrophes\' `ticks!`");
  // expect("No matched quotes", 1, "");

  // tokens = lex("\"Escaped \\\"quote\\\"\" + \"in here\"");
  // expect("String with escaped characters", 1, "");

  // tokens = lex("what: 4; hi: 2;");
  // expect("Identifiers", 1, "");

  // tokens = lex("if else 123 not a keyword");
  // expect("Keywords", 1, "");

  // tokens = lex("ifnt elsent 123 not a keyword");
  // expect("Keywords must be exact", 1, "");

  // tokens = lex("$23o@ a@#F: -2@#; \\ i??jawoea \"wef:3j\" ");
  // expect("Not sure what to do here?", 1, "");

  Node *nodes = parse(lex("1 + 8.43 - 123.4382 - 230492380 / 20390.12312 + 12"));
  expect("Number node", nodes[1].type == BinaryExpression, "Not binary.")

      printf("-----------------------\n");
  printf("Total tests: %d (\e[32m%d\e[0m passed,  \e[31m%d\e[0m failed)\n\n",
         TESTS_TOTAL, TESTS_PASSED, TESTS_FAILED);
  return 0;
}
