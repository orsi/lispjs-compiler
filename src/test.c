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
  Token *tokens;
  Node *node;

  tokens = lex(read_filepath("./src/mock/literals-identifiers.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nliterals-identifiers.rox\n");
  print_program(node);

  tokens = lex(read_filepath("./src/mock/literals-keywords.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nliterals-keywords.rox\n");
  print_program(node);

  tokens = lex(read_filepath("./src/mock/literals-numbers.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nliterals.rox\n");
  print_program(node);

  tokens = lex(read_filepath("./src/mock/literals-strings.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nliterals-strings.rox\n");
  print_program(node);

  tokens = lex(read_filepath("./src/mock/expressions-binary.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nexpressions-binary.rox\n");
  print_program(node);

  tokens = lex(read_filepath("./src/mock/expressions-assignment.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nexpressions-assignment.rox\n");
  print_program(node);

  tokens = lex(read_filepath("./src/mock/arrays.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\narrays.rox\n");
  print_program(node);

  tokens = lex(read_filepath("./src/mock/blocks.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nblocks.rox\n");
  print_program(node);

  tokens = lex(read_filepath("./src/mock/functions.rox"));
  node = create_node(NODE_PROGRAM, tokens, NULL);
  node->body = parse(tokens, NULL);
  printf("\nfunctions.rox\n");
  print_program(node);

  print_test_results();
}
