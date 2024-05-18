#include "./roxanne.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int TESTS_TOTAL = 0;
static int TESTS_PASSED = 0;
static int TESTS_FAILED = 0;

#define expect(condition) _expect(condition, #condition, sizeof(#condition))

void _expect(bool expression, char *description, int description_length) {
  TESTS_TOTAL++;
  if (expression) {
    TESTS_PASSED++;
    printf("%d: \x1b[32m✔ Passed\x1b[0m %.*s \n", TESTS_TOTAL,
           description_length, description);
  } else {
    TESTS_FAILED++;
    printf("%d: \x1b[31m✖ Failed\x1b[0m %.*s\n", TESTS_TOTAL,
           description_length, description);
  }
}

int compare_lex_to_string(char *statement, char *string) {
  return strcmp(get_token_string(lex(statement)), string) == 0;
}

int compare_parse_to_string(char *statement, char *string) {
  return strcmp(get_node_string(parse(lex(statement))), string) == 0;
}

int compare_eval_to_string(char *statement, char *string) {
  return strcmp(get_result_string(evaluate_node(parse(lex(statement)))),
                string) == 0;
}

void print_test_results() {
  printf("\n-- Test Results --\n");
  printf("Total:\t%d\n", TESTS_TOTAL);
  printf("Passed:\t\e[32m%d\e[0m\n", TESTS_PASSED);
  printf("Failed:\t\e[31m%d\e[0m\n\n", TESTS_FAILED);
}

int main(void) {
  // lexer tests
  expect(compare_lex_to_string("// don't parse me!\n// or me!", "null"));
  expect(compare_lex_to_string("hi", "id: hi"));
  expect(compare_lex_to_string("if", "key: if"));
  expect(compare_lex_to_string("123.123", "num: 123.123"));
  expect(compare_lex_to_string("\"hi\"", "str: hi"));
  expect(compare_lex_to_string(":", "sym: :"));

  // parse tests
  expect(compare_parse_to_string("1", "int: 1"));
  expect(compare_parse_to_string("\"hi\"", "str: hi"));
  expect(compare_parse_to_string("129.012", "dbl: 129.012000"));
  expect(compare_parse_to_string("11 + 12.4", "bin: +"));
  expect(compare_parse_to_string("test", "var: test"));

  // eval tests
  expect(compare_eval_to_string("1 + 2 * 3 + 4 + 5 * 6 * 7 + 8", "int: 229"));
  expect(compare_eval_to_string("2 * (3 + 4)", "int: 14"));
  expect(compare_eval_to_string("250.0 * 1.5", "double: 375.000000"));
  expect(compare_eval_to_string("12938112.01224234233123 / 234.5122342342343",
                                "double: 55170.307231"));
  expect(compare_eval_to_string("1 / 2 + 3 - 4 % 5", "double: -0.500000"));
  expect(compare_eval_to_string("hi: 1", "none"));
  expect(compare_eval_to_string(read_filepath("./src/mock/test.rox"),
                                "double: 2.500000"));

  print_test_results();
}
