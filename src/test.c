#include "./lex.c"
#include "./parse.c"
#include <assert.h>
#include <stdio.h>
#include <string.h>

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
  }

static int TESTS_TOTAL = 0;
static int TESTS_PASSED = 0;
static int TESTS_FAILED = 0;

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

  char *node_str = "1 + 2 * 3 + 4 + 5 * 6 * 7 + 8";
  Node *root_node = parse(lex(node_str));
  int level = 0;
  expect("Parse: 1 + 2 * 3 + 4 + 5 * 6 * 7 + 8",
         root_node->left->left->left->right->left->int_value == 2, "Not 2.");
  print_node_tree(root_node, level, "");

  printf("-----------------------\n");
  printf("Total tests: %d (\e[32m%d\e[0m passed,  \e[31m%d\e[0m failed)\n\n",
         TESTS_TOTAL, TESTS_PASSED, TESTS_FAILED);
  return 0;
}
