#include "roxanne.h"
#include <stdio.h>

static int TESTS_TOTAL = 0;
static int TESTS_PASSED = 0;
static int TESTS_FAILED = 0;

#define expect(actual, expected) _expect(actual, expected)

void _expect(char *actual, char *expected) {
  TESTS_TOTAL++;
  if (strcmp(expected, actual) == 0) {
    TESTS_PASSED++;
    printf("%-3d \x1b[32m✔ Passed\x1b[0m %s \x1b[44m %.*s \x1b[0m\n",
           TESTS_TOTAL, expected, (int)strlen(actual), actual);
  } else {
    TESTS_FAILED++;
    printf("%-3d \x1b[31m✖ Failed\x1b[0m %s \x1b[44m %.*s \x1b[0m\n",
           TESTS_TOTAL, expected, (int)strlen(actual), actual);
  }
}

#define expect_number(actual, expected) _expect_number(actual, expected)

void _expect_number(double actual, double expected) {
  TESTS_TOTAL++;
  if (expected == actual) {
    TESTS_PASSED++;
    printf("%-3d \x1b[32m✔ Passed\x1b[0m %16f \x1b[44m %16f \x1b[0m\n",
           TESTS_TOTAL, expected, actual);
  } else {
    TESTS_FAILED++;
    printf("%-3d \x1b[31m✖ Failed\x1b[0m %16f \x1b[44m %16f \x1b[0m\n",
           TESTS_TOTAL, expected, actual);
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
  expect(get_node_string(get_array_at(parse(lex("1"))->statements, 0)),
         "node:number, 1");
  expect(get_node_string(get_array_at(parse(lex("1.0"))->statements, 0)),
         "node:number, 1");
  expect(get_node_string(get_array_at(parse(lex("-1.0"))->statements, 0)),
         "node:binary, -");
  expect(get_node_string(get_array_at(parse(lex("0"))->statements, 0)),
         "node:number, 0");
  // TODO: Should force always leading digit?
  expect(get_node_string(get_array_at(parse(lex(".0"))->statements, 0)),
         "node:number, 0");
  expect(get_node_string(get_array_at(parse(lex("-0"))->statements, 0)),
         "node:binary, -");
  expect(get_node_string(get_array_at(parse(lex("-.0"))->statements, 0)),
         "node:binary, -");

  // test double precision up to 16 total digits
  expect_number(
      ((Node *)get_array_at(parse(lex("123456789.123456786"))->statements, 0))
          ->number,
      123456789.123456786);
  expect(get_node_string(
             get_array_at(parse(lex("123456789.12345675"))->statements, 0)),
         "node:number, 123456789.1234567");
  expect(get_node_string(
             get_array_at(parse(lex("99999999.99999999"))->statements, 0)),
         "node:number, 99999999.99999999");
  expect(get_node_string(
             get_array_at(parse(lex("99999999.999999999"))->statements, 0)),
         "node:number, 100000000");

  expect(get_node_string(get_array_at(parse(lex(("\"hi\"")))->statements, 0)),
         "node:string, hi");
  expect(
      get_node_string(get_array_at(parse(lex(("11 + 12.4")))->statements, 0)),
      "node:binary, +");
  expect(get_node_string(get_array_at(parse(lex(("test")))->statements, 0)),
         "node:identifier, test");

  expect(get_node_string(get_array_at(parse(lex(("hello: 1")))->statements, 0)),
         "node:assignment, :");
  expect(get_node_string(get_array_at(
             parse(lex(("if (1 + 2) * 3 > 1 {\n //\n } else {}")))->statements,
             0)),
         "node:conditional, node:binary, >");

  // precedence
  expect(
      get_node_string(get_array_at(parse(lex(("(1 + 2) * 3")))->statements, 0)),
      "node:binary, *");
  expect(get_node_string(
             ((Node *)get_array_at(parse(lex(("1 + 2 * 3")))->statements, 0))
                 ->right),
         "node:binary, *");

  print_test_results();
}
