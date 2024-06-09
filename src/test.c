#include "roxanne.h"
#include <stdio.h>

static int TESTS_TOTAL = 0;
static int TESTS_PASSED = 0;
static int TESTS_FAILED = 0;

#define expect(actual, expected) _expect(actual, expected, __LINE__)

void _expect(const char *actual, const char *expected, int line) {
  TESTS_TOTAL++;
  if (strcmp(expected, actual) == 0) {
    TESTS_PASSED++;
    printf("%-3d (line: %d) \x1b[32m✔ Passed\x1b[0m %s \x1b[44m %.*s \x1b[0m\n",
           TESTS_TOTAL, line, expected, (int)strlen(actual), actual);
  } else {
    TESTS_FAILED++;
    printf("%-3d (line: %d) \x1b[31m✖ Failed\x1b[0m %s \x1b[44m %.*s \x1b[0m\n",
           TESTS_TOTAL, line, expected, (int)strlen(actual), actual);
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

void print_test_results(void) {
  printf("\n-- Test Results --\n");
  printf("Total:\t%d\n", TESTS_TOTAL);
  printf("Passed:\t\x1b[32m%d\x1b[0m\n", TESTS_PASSED);
  printf("Failed:\t\x1b[31m%d\x1b[0m\n\n", TESTS_FAILED);
}

void test_lex(void) {
  Token *tokens;
  expect(stringify_token(lex("// don't parse me!\n// or me!")), "token:eof");
  expect(stringify_token(lex("123.123")), "token:number:123.123");
  expect(stringify_token(lex("\"hi\"")), "token:string:hi");
  expect(stringify_token(lex("if")), "token:id:if");
  expect(stringify_token(lex("else")), "token:id:else");
  expect(stringify_token(lex("return")), "token:id:return");
  expect(stringify_token(lex("import")), "token:id:import");
  expect(stringify_token(lex("hi")), "token:id:hi");
  expect(stringify_token(lex(";")), "token:symbol:;");
  expect(stringify_token(lex(":")), "token:symbol::");
  tokens = lex(read_filepath("./src/mock/test2.rox"));
  expect(stringify_token(tokens), "token:id:import");
  Token *last_token;
  while (tokens) {
    last_token = tokens;
    tokens = tokens->next;
  }
  expect(stringify_token(last_token), "token:eof");
}

void test_parse(void) {
  expect(stringify_node(parse_program(lex("1"))->body), "node:number:1");
  expect(stringify_node(parse_program(lex("1.0"))->body), "node:number:1");
  expect(stringify_node(parse_program(lex("-1.0"))->body), "node:binary:-1");
  expect(stringify_node(parse_program(lex("0"))->body), "node:number:0");
  // TODO: Should force always leading digit?
  expect(stringify_node(parse_program(lex(".0"))->body), "node:number:0");
  expect(stringify_node(parse_program(lex("-0"))->body), "node:binary:-0");
  expect(stringify_node(parse_program(lex("-.0"))->body), "node:binary:-.0");

  // test double precision up to 16 total digits
  expect_number(parse_program(lex("123456789.123456786"))->body->number,
                123456789.123456786);
  expect(stringify_node(parse_program(lex("123456789.12345675"))->body),
         "node:number:123456789.1234567");
  expect(stringify_node(parse_program(lex("99999999.99999999"))->body),
         "node:number:99999999.99999999");
  expect(stringify_node(parse_program(lex("99999999.999999999"))->body),
         "node:number:100000000");

  expect(stringify_node(parse_program(lex(("\"hi\"")))->body),
         "node:string:\"hi\"");
  expect(stringify_node(parse_program(lex(("11 + 12.4")))->body),
         "node:binary:11+12.4");
  expect(stringify_node(parse_program(lex(("test")))->body),
         "node:identifier:test");

  expect(stringify_node(parse_program(lex(("hello: 1")))->body),
         "node:assignment:hello:1");
  expect(
      stringify_node(
          parse_program(lex(("if (1 + 2) * 3 > 1 {\n //\n } else {}")))->body),
      "node:conditional:node:binary, >");

  // precedence
  expect(stringify_node(parse_program(lex(("(1 + 2) * 3")))->body),
         "node:binary:*");
  expect(stringify_node(parse_program(lex(("1 + 2 * 3")))->body->right),
         "node:binary:2*3");
}

void test_evaluate(void) {
  Node *program = parse_program(lex(read_filepath("./src/mock/literals.rox")));
  Node *current = program->body;
  expect(stringify_result(evaluate(current)), "result:number:1.000000");
  current = current->next;
  expect(stringify_result(evaluate(current)), "result:number:1.000000");
  current = current->next;
  expect(stringify_result(evaluate(current)), "result:number:10000.000000");
  current = current->next;
  expect(stringify_result(evaluate(current)), "result:number:10000.000000");
  current = current->next;
  expect(stringify_result(evaluate(current)), "result:number:6.000000");
  current = current->next;
  expect(stringify_result(evaluate(current)),
         "result:number:81985529216486896.000000");
  current = current->next;
  expect(stringify_result(evaluate(current)), "result:number:342391.000000");
  current = current->next;
  expect(stringify_result(evaluate(current)), "result:string:\"hello!\"");
  current = current->next;
  expect(stringify_result(evaluate(current)),
         "result:string:\"result 7 should be 7 yo\"");
  current = current->next;
  expect(stringify_result(evaluate(current)),
         "result:string:\"string interpolation wat\"");
  current = current->next;
  expect(stringify_result(evaluate(current)), "result:boolean:true");
  current = current->next;
  expect(stringify_result(evaluate(current)), "result:boolean:false");
  current = current->next;
  expect(stringify_result(evaluate(current)),
         "result:array:[1, true, 1, \"hello!\"]");
}

int main(void) {
  test_lex();
  test_parse();
  test_evaluate();

  Token *tokens;
  tokens = lex(read_filepath("./src/mock/object-literals.rox"));
  Node *node = parse_program(tokens);
  printf("\nprogram\n");
  print_program(node);

  //   tokens = lex(read_filepath("./src/mock/string-interpolation.rox"));
  //   printf("\nprogram\n");
  //   program = parse(tokens);
  //   print_program(program);

  //   tokens = lex(read_filepath("./src/mock/functions.rox"));
  //   printf("\nprogram\n");
  //   program = parse(tokens);
  //   print_program(program);

  print_test_results();
}
