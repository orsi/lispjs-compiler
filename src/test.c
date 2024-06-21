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

// void test_parse(void) {
//   expect(stringify_node(create_node(NODE_PROGRAM, parse(lex("1"),
//   NULL))->body),
//          "node:number:1");
//   expect(
//       stringify_node(create_node(NODE_PROGRAM, parse(lex("1.0"),
//       NULL))->body), "node:number:1");
//   expect(
//       stringify_node(create_node(NODE_PROGRAM, parse(lex("-1.0"),
//       NULL))->body), "node:binary:-1");
//   expect(stringify_node(create_node(NODE_PROGRAM, parse(lex("0"),
//   NULL))->body),
//          "node:number:0");
//   // TODO: Should force always leading digit?
//   expect(
//       stringify_node(create_node(NODE_PROGRAM, parse(lex(".0"),
//       NULL))->body), "node:number:0");
//   expect(
//       stringify_node(create_node(NODE_PROGRAM, parse(lex("-0"),
//       NULL))->body), "node:binary:-0");
//   expect(
//       stringify_node(create_node(NODE_PROGRAM, parse(lex("-.0"),
//       NULL))->body), "node:binary:-.0");

//   // test double precision up to 16 total digits
//   expect_number(
//       create_node(NODE_PROGRAM, parse(lex("123456789.123456786"), NULL))
//           ->body->number,
//       123456789.123456786);
//   expect(stringify_node(
//              create_node(NODE_PROGRAM, parse(lex("123456789.12345675"),
//              NULL))
//                  ->body),
//          "node:number:123456789.1234567");
//   expect(stringify_node(
//              create_node(NODE_PROGRAM, parse(lex("99999999.99999999"), NULL))
//                  ->body),
//          "node:number:99999999.99999999");
//   expect(stringify_node(
//              create_node(NODE_PROGRAM, parse(lex("99999999.999999999"),
//              NULL))
//                  ->body),
//          "node:number:100000000");

//   expect(stringify_node(
//              create_node(NODE_PROGRAM, parse(lex(("\"hi\"")), NULL))->body),
//          "node:string:\"hi\"");
//   expect(stringify_node(
//              create_node(NODE_PROGRAM, parse(lex(("11 + 12.4")),
//              NULL))->body),
//          "node:binary:11+12.4");
//   expect(stringify_node(
//              create_node(NODE_PROGRAM, parse(lex(("test")), NULL))->body),
//          "node:identifier:test");

//   expect(stringify_node(
//              create_node(NODE_PROGRAM, parse(lex(("hello: 1")),
//              NULL))->body),
//          "node:assignment:hello:1");
//   expect(stringify_node(
//              create_node(
//                  NODE_PROGRAM,
//                  parse(lex(("if (1 + 2) * 3 > 1 {\n //\n } else {}")), NULL))
//                  ->body),
//          "node:conditional:node:binary, >");

//   // precedence
//   expect(
//       stringify_node(
//           create_node(NODE_PROGRAM, parse(lex(("(1 + 2) * 3")),
//           NULL))->body),
//       "node:binary:*");
//   expect(
//       stringify_node(create_node(NODE_PROGRAM, parse(lex(("1 + 2 * 3")),
//       NULL))
//                          ->body->right),
//       "node:binary:2*3");
// }

// void test_evaluate(void) {
//   Node *program = parse(lex(read_filepath("./src/mock/literals.rox")), NULL);
//   Node *current = program->body;
//   expect(stringify_result(evaluate(current)), "result:number:1.000000");
//   current = current->next;
//   expect(stringify_result(evaluate(current)), "result:number:1.000000");
//   current = current->next;
//   expect(stringify_result(evaluate(current)), "result:number:10000.000000");
//   current = current->next;
//   expect(stringify_result(evaluate(current)), "result:number:10000.000000");
//   current = current->next;
//   expect(stringify_result(evaluate(current)), "result:number:6.000000");
//   current = current->next;
//   expect(stringify_result(evaluate(current)),
//          "result:number:81985529216486896.000000");
//   current = current->next;
//   expect(stringify_result(evaluate(current)), "result:number:342391.000000");
//   current = current->next;
//   expect(stringify_result(evaluate(current)), "result:string:\"hello!\"");
//   current = current->next;
//   expect(stringify_result(evaluate(current)),
//          "result:string:\"result 7 should be 7 yo\"");
//   current = current->next;
//   expect(stringify_result(evaluate(current)),
//          "result:string:\"string interpolation wat\"");
//   current = current->next;
//   expect(stringify_result(evaluate(current)), "result:boolean:true");
//   current = current->next;
//   expect(stringify_result(evaluate(current)), "result:boolean:false");
//   current = current->next;
//   expect(stringify_result(evaluate(current)),
//          "result:array:[1, true, 1, \"hello!\"]");
// }

int main(void) {
  //   test_lex();
  //   test_parse();
  //   test_evaluate();

  Token *tokens;
  Node *node;

  // tokens = lex(read_filepath("./src/mock/literals-identifiers.rox"));
  // node = create_node(NODE_PROGRAM, tokens, NULL);
  // node->body = parse(tokens, NULL);
  // printf("\nliterals-identifiers.rox\n");
  // print_program(node);

  tokens = lex(read_filepath("./src/mock/literals-keywords.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nliterals-keywords.rox\n");
  print_program(node);

  // tokens = lex(read_filepath("./src/mock/literals-numbers.rox"));
  // node = create_node(NODE_PROGRAM, parse(tokens, NULL));
  // printf("\nliterals.rox\n");
  // print_program(node);

  // tokens = lex(read_filepath("./src/mock/literals-strings.rox"));
  // node = create_node(NODE_PROGRAM, parse(tokens, NULL));
  // printf("\nliterals-strings.rox\n");
  // print_program(node);

  // tokens = lex(read_filepath("./src/mock/expressions-binary.rox"));
  // node = create_node(NODE_PROGRAM, parse(tokens, NULL));
  // printf("\nexpressions-binary.rox\n");
  // print_program(node);

  // tokens = lex(read_filepath("./src/mock/expressions-assignment.rox"));
  // node = create_node(NODE_PROGRAM, parse(tokens, NULL));
  // printf("\nexpressions-assignment.rox\n");
  // print_program(node);

  // tokens = lex(read_filepath("./src/mock/arrays.rox"));
  // node = create_node(NODE_PROGRAM, tokens, NULL);
  // node->body = parse(tokens, NULL);
  // printf("\narrays.rox\n");
  // print_program(node);

  // tokens = lex(read_filepath("./src/mock/blocks.rox"));
  // node = create_node(NODE_PROGRAM, tokens, NULL);
  // node->body = parse(tokens, NULL);
  // printf("\nblocks.rox\n");
  // print_program(node);

  //   tokens = lex(read_filepath("./src/mock/functions.rox"));
  //   node = create_node(NODE_PROGRAM, parse(tokens, NULL));
  //   printf("\nfunctions.rox\n");
  //   print_program(node);

  print_test_results();
}
