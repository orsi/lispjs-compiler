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

#define expect(actual, expected) _expect(actual, expected)

void _expect(char *actual, char *expected) {
  TESTS_TOTAL++;
  if (strcmp(expected, actual) == 0) {
    TESTS_PASSED++;
    printf("%-3d \x1b[32m✔ Passed\x1b[0m \"%s\"\n", TESTS_TOTAL, expected);
  } else {
    TESTS_FAILED++;
    printf("%-3d \x1b[31m✖ Failed\x1b[0m \"%s\"\n\tactual: \"%.*s\"\n",
           TESTS_TOTAL, expected, (int)strlen(actual), actual);
  }
}

void print_test_results() {
  printf("\n-- Test Results --\n");
  printf("Total:\t%d\n", TESTS_TOTAL);
  printf("Passed:\t\e[32m%d\e[0m\n", TESTS_PASSED);
  printf("Failed:\t\e[31m%d\e[0m\n\n", TESTS_FAILED);
}

int main(void) {
  // lexer tests
  expect(get_token_string(lex("// don't parse me!\n// or me!")), "token:eof");
  expect(get_token_string(lex("123.123")), "token:number, 123.123");
  expect(get_token_string(lex("\"hi\"")), "token:string, hi");
  expect(get_token_string(lex("if")), "token:keyword, if");
  expect(get_token_string(lex("else")), "token:keyword, else");
  expect(get_token_string(lex("return")), "token:keyword, return");
  expect(get_token_string(lex("import")), "token:keyword, import");
  expect(get_token_string(lex("hi")), "token:id, hi");
  expect(get_token_string(lex(";")), "token:symbol, ;");
  expect(get_token_string(lex(":")), "token:symbol, :");
  Token *tokens = lex(read_filepath("./src/mock/test2.rox"));
  expect(get_token_string(tokens), "token:keyword, import");
  Token *last_token;
  while (tokens) {
    last_token = tokens;
    tokens = tokens->next;
  }
  expect(get_token_string(last_token), "token:eof");

  // parse tests
  expect(get_node_string(parse(lex(("1")))->statements->items[0]),
         "node:integer, 1");
  expect(get_node_string(parse(lex(("129.012")))->statements->items[0]),
         "node:double, 129.012000");
  expect(get_node_string(parse(lex(("\"hi\"")))->statements->items[0]),
         "node:string, hi");
  expect(get_node_string(parse(lex(("11 + 12.4")))->statements->items[0]),
         "node:binary, +");
  expect(get_node_string(parse(lex(("test")))->statements->items[0]),
         "node:variable, test");
  expect(get_node_string(parse(lex(("hello: 1")))->statements->items[0]),
         "node:assignment, :");
  expect(get_node_string(parse(lex(("(1 + 2) * 3")))->statements->items[0]),
         "node:binary, +");
  expect(
      get_node_string(
          parse(lex(("if (1 + 2) * 3 > 1 {\n //\n } else { }")))->statements->items[0]),
      "node:conditional, node:binary, >");

  // eval tests
  //   expect(stringify_evaluate("hi: 1"), "result:none");
  //   expect(stringify_evaluate("1 + 2 * 3 + 4 + 5 * 6 * 7 + 8"),
  //          "result:integer, 229");
  //   expect(stringify_evaluate("2 * (3 + 4)"), "result:integer, 14");
  //   expect(stringify_evaluate("250.0 * 1.5"), "result:double, 375.000000");
  //   expect(stringify_evaluate("12938112.01224234233123 /
  //   234.5122342342343"),
  //          "result:double, 55170.307231");
  //   expect(stringify_evaluate("1 / 2 + 3 - 4 % 5"),
  //          "result:double,
  //              - 0.500000 "); expect(stringify_evaluate("\"hello!\""),
  //       "result:string,
  //       hello !");

  // program tests
  //   expect(stringify_parse(read_filepath("./src/mock/test.rox")),
  //   "result:none");
  //   Program program;
  //   parse_program(lex(read_filepath("./src/mock/test.rox")), &program);

  print_test_results();
}
