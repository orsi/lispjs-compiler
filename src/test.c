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

int main(void) {
  //   // lexer tests
  //   expect(stringify_token(lex("// don't parse me!\n// or me!")),
  //   "token:eof"); expect(stringify_token(lex("123.123")),
  //   "token:number:123.123"); expect(stringify_token(lex("\"hi\"")),
  //   "token:string:hi"); expect(stringify_token(lex("if")), "token:id:if");
  //   expect(stringify_token(lex("else")), "token:id:else");
  //   expect(stringify_token(lex("return")), "token:id:return");
  //   expect(stringify_token(lex("import")), "token:id:import");
  //   expect(stringify_token(lex("hi")), "token:id:hi");
  //   expect(stringify_token(lex(";")), "token:symbol:;");
  //   expect(stringify_token(lex(":")), "token:symbol::");
  //   Token *tokens = lex(read_filepath("./src/mock/test2.rox"));
  //   expect(stringify_token(tokens), "token:id:import");
  //   Token *last_token;
  //   while (tokens) {
  //     last_token = tokens;
  //     tokens = tokens->next;
  //   }
  //   expect(stringify_token(last_token), "token:eof");

  //   // parse tests
  //   expect(stringify_node(get_array_item_at(parse(lex("1"))->statements, 0)),
  //          "node:number:1");
  //   expect(stringify_node(get_array_item_at(parse(lex("1.0"))->statements,
  //   0)),
  //          "node:number:1");
  //   expect(stringify_node(get_array_item_at(parse(lex("-1.0"))->statements,
  //   0)),
  //          "node:binary:-1");
  //   expect(stringify_node(get_array_item_at(parse(lex("0"))->statements, 0)),
  //          "node:number:0");
  //   // TODO: Should force always leading digit?
  //   expect(stringify_node(get_array_item_at(parse(lex(".0"))->statements,
  //   0)),
  //          "node:number:.0");
  //   expect(stringify_node(get_array_item_at(parse(lex("-0"))->statements,
  //   0)),
  //          "node:binary:-0");
  //   expect(stringify_node(get_array_item_at(parse(lex("-.0"))->statements,
  //   0)),
  //          "node:binary:-.0");

  //   // test double precision up to 16 total digits
  //   expect_number(((Node *)get_array_item_at(
  //                      parse(lex("123456789.123456786"))->statements, 0))
  //                     ->number,
  //                 123456789.123456786);
  //   expect(stringify_node(get_array_item_at(
  //              parse(lex("123456789.12345675"))->statements, 0)),
  //          "node:number:123456789.1234567");
  //   expect(stringify_node(
  //              get_array_item_at(parse(lex("99999999.99999999"))->statements,
  //              0)),
  //          "node:number:99999999.99999999");
  //   expect(stringify_node(get_array_item_at(
  //              parse(lex("99999999.999999999"))->statements, 0)),
  //          "node:number:100000000");

  //   expect(
  //       stringify_node(get_array_item_at(parse(lex(("\"hi\"")))->statements,
  //       0)), "node:string:\"hi\"");
  //   expect(stringify_node(
  //              get_array_item_at(parse(lex(("11 + 12.4")))->statements, 0)),
  //          "node:binary:11+12.4");
  //   expect(stringify_node(get_array_item_at(parse(lex(("test")))->statements,
  //   0)),
  //          "node:identifier:test");

  //   expect(stringify_node(
  //              get_array_item_at(parse(lex(("hello: 1")))->statements, 0)),
  //          "node:assignment:hello:1");
  //   expect(stringify_node(get_array_item_at(
  //              parse(lex(("if (1 + 2) * 3 > 1 {\n //\n } else
  //              {}")))->statements, 0)),
  //          "node:conditional:node:binary, >");

  //   // precedence
  //   expect(stringify_node(
  //              get_array_item_at(parse(lex(("(1 + 2) * 3")))->statements,
  //              0)),
  //          "node:binary:*");
  //   expect(stringify_node(((Node *)get_array_item_at(
  //                              parse(lex(("1 + 2 * 3")))->statements, 0))
  //                             ->right),
  //          "node:binary:2*3");

  //   Program *program = parse(lex(read_filepath("./src/mock/literals.rox")));
  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   0))),
  //          "result:number:1.000000");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   1))),
  //          "result:number:1.000000");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   2))),
  //          "result:number:10000.000000");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   3))),
  //          "result:number:10000.000000");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   4))),
  //          "result:number:6.000000");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   5))),
  //          "result:number:81985529216486896.000000");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   6))),
  //          "result:number:342391.000000");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   7))),
  //          "result:string:\"hello!\"");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   8))),
  //          "result:string:\"result 7 should be 7 yo\"");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   9))),
  //          "result:string:\"string interpolation wat\"");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   10))),
  //          "result:boolean:true");

  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   11))),
  //          "result:boolean:false");
  //   expect(stringify_result(evaluate(get_array_item_at(program->statements,
  //   12))),
  //          "result:array:[1, true, 1, \"hello!\"]");

  //   printf("\ntokens\n");
  //   tokens = lex(read_filepath("./src/mock/object-literals.rox"));
  //   print_tokens(tokens);

  //   printf("\nprogram\n");
  //   program = parse(tokens);
  //   print_program(program);

  printf("\ntokens\n");
  Token *tokens = lex(read_filepath("./src/mock/string-interpolation.rox"));
  print_tokens(tokens);

  printf("\nprogram\n");
  Program *program = parse(tokens);
  print_program(program);

  print_test_results();
}
