#include "./evaluate.h"
#include "./lex.h"
#include "./parse.h"
#include <math.h>
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
  Token *tokens = lex("// don't parse me!\n// or me!");
  expect("Comments //", tokens == NULL, "So what.");

  tokens = lex("/* don't parse me!\nor me!*/");
  expect("Comments: /* */", tokens == NULL, "Ya I failed.");

  tokens = lex("/* don't parse me!\nor me!*/\nhi: 123.123 /* yup */");
  expect("Comments with variables afterwards", tokens != NULL, "");

  tokens = lex("1 + 2 + 3");
  expect("Simple expression like 1+2+3", tokens[0].type == TOKEN_NUMBER, "");

  tokens =
      lex("909450945082 + .123190024234 - 1231231249.34989283423 + 00123.123");
  expect("Numbers, decimals", 1, "");

  tokens = lex("\"quotes\" \'apostrophes\' `ticks!`");
  expect("Quotes and string literals (`, ', \")", 1, "");

  tokens = lex("\"quotes \'apostrophes\' `ticks!`");
  expect("No matched quotes", 1, "");

  tokens = lex("\"Escaped \\\"quote\\\"\" + \"in here\"");
  expect("String with escaped characters", 1, "");

  tokens = lex("what: 4; hi: 2;");
  expect("Identifiers", 1, "");

  tokens = lex("if else 123 not a keyword");
  expect("Keywords", 1, "");

  tokens = lex("ifnt elsent 123 not a keyword");
  expect("Keywords must be exact", 1, "");

  tokens = lex("$23o@ a@#F: -2@#; \\ i??jawoea \"wef:3j\" ");
  expect("Not sure what to do here?", 1, "");

  char *node_str = "1 + 2 * 3 + 4 + 5 * 6 * 7 + 8";
  Node *root_node = parse(lex(node_str));
  Result *result = evaluate_node(root_node);
  expect("Parse: 1 + 2 * 3 + 4 + 5 * 6 * 7 + 8", result->int_value == 229,
         "Not 229");

  result = evaluate_node(parse(lex("1 + 2")));
  expect("Test 1", result->type == RESULT_INTEGER && result->int_value == 3,
         "Not integer");

  result = evaluate_node(parse(lex("2 * 3")));
  expect("Test 2", result->type == RESULT_INTEGER && result->int_value == 6,
         "Not integer");

  result = evaluate_node(parse(lex("250.0 * 1.5")));
  expect("Test 3",
         result->type == RESULT_DOUBLE && result->double_value == 375.000000,
         "Not double");

  result =
      evaluate_node(parse(lex("12938112.01224234233123 / 234.5122342342343")));
  expect("Test 4 type", result->type == RESULT_DOUBLE, "Not double");
  expect("Test 4 value", fabs(result->double_value - 55170.307231) < 0.000001,
         "Not 55170.307231");

  tokens = lex("1 / 2 + 3 - 4 % 5");
  Node *node_tree = parse(tokens);
  result = evaluate_node(node_tree);
  expect("Test 5 type", result->type == RESULT_DOUBLE, "Not double");
  expect("Test 5 value", fabs(result->double_value - -0.500000) < 0.000001,
         "Not -0.500000");

  tokens = lex("2 * (3 + 4)");
  node_tree = parse(tokens);
  result = evaluate_node(node_tree);
  expect("Test 6 type", result->type == RESULT_INTEGER, "Not integer");
  expect("Test 6 value", result->int_value == 14, "Not 14");

  printf("-----------------------\n");
  printf("Total tests: %d (\e[32m%d\e[0m passed,  \e[31m%d\e[0m failed)\n\n",
         TESTS_TOTAL, TESTS_PASSED, TESTS_FAILED);
  return 0;
}
