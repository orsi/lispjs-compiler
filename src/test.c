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

void print_test_results() {
  printf("\n-- Test Results --\n");
  printf("Total:\t%d\n", TESTS_TOTAL);
  printf("Passed:\t\e[32m%d\e[0m\n", TESTS_PASSED);
  printf("Failed:\t\e[31m%d\e[0m\n\n", TESTS_FAILED);
}

int main(void) {
  expect(lex("// don't parse me!\n// or me!") == NULL);
  expect(lex("/* don't parse me!\nor me!*/") == NULL);
  expect(lex("/* don't parse me!\nor me!*/\nhi: 123.123 /* yup */") != NULL);
  expect(lex("1 + 2 + 3")->type == TOKEN_NUMBER);

  //   tokens =
  //       lex("909450945082 + .123190024234 - 1231231249.34989283423 +
  //       00123.123");
  //   expect(1);

  //   tokens = lex("\"quotes\" \'apostrophes\' `ticks!`");
  //   expect(1);

  //   tokens = lex("\"quotes \'apostrophes\' `ticks!`");
  //   expect(1);

  //   tokens = lex("\"Escaped \\\"quote\\\"\" + \"in here\"");
  //   expect(1);

  //   tokens = lex("what: 4; hi: 2;");
  //   expect(1);

  //   tokens = lex("if else 123 not a keyword");
  //   expect(1);

  //   tokens = lex("ifnt elsent 123 not a keyword");
  //   expect(1);

  //   tokens = lex("$23o@ a@#F: -2@#; \\ i??jawoea \"wef:3j\" ");
  //   expect(1);

  Result *result = evaluate_node(parse(lex("1 + 2 * 3 + 4 + 5 * 6 * 7 + 8")));
  expect(result->int_value == 229);

  result = evaluate_node(parse(lex("1 + 2")));
  expect(result->type == RESULT_INTEGER && result->int_value == 3);

  result = evaluate_node(parse(lex("2 * 3")));
  expect(result->type == RESULT_INTEGER && result->int_value == 6);

  result = evaluate_node(parse(lex("250.0 * 1.5")));
  expect(result->type == RESULT_DOUBLE && result->double_value == 375.000000);

  result =
      evaluate_node(parse(lex("12938112.01224234233123 / 234.5122342342343")));
  expect(result->type == RESULT_DOUBLE);
  expect(fabs(result->double_value - 55170.307231) < 0.000001);

  result = evaluate_node(parse(lex("1 / 2 + 3 - 4 % 5")));
  expect(result->type == RESULT_DOUBLE);
  expect(fabs(result->double_value - -0.500000) < 0.000001);

  result = evaluate_node(parse(lex("2 * (3 + 4)")));
  expect(result->type == RESULT_INTEGER);
  expect(result->int_value == 14);

  result = evaluate_node(parse(lex(read_filepath("./src/mock/test.rox"))));
  expect(result->double_value == 2.5);

  print_test_results();
}
