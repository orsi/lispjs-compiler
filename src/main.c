#include "evaluate.h"
#include "lex.h"
#include "parse.h"
#include "utils.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1

static volatile bool is_running = true;

void on_interrupt_handler(int signal) {
  printf("caught.\n");
  is_running = false;
}

int main(int argc, char *argv[]) {

  if (DEBUG) {
    printf("arg count %d\n", argc);
    for (int i = 0; i < argc; i++) {
      printf("%d: %s\n", (int)i, argv[i]);
    }
  }

  // repl if no command given
  if (argc == 1) {
    signal(SIGINT, &on_interrupt_handler);
    signal(SIGQUIT, &on_interrupt_handler);

    char *text = calloc(0, 1);
    char buffer[500];
    while (is_running) {
      printf("rox> ");

      // get input
      fgets(buffer, sizeof(buffer), stdin);
      size_t length = strlen(buffer);
      text = calloc(0, length + 1);
      if (text == NULL) {
        printf("Error malloc text");
        continue;
      }
      strncpy(text, buffer, length);

      // exit user input
      if (starts_with(text, "exit")) {
        is_running = false;
        break;
      }

      Token *tokens = lex(text);
      Node *node = parse(tokens, NULL);
      Result *result = evaluate(node);
      printf("%s\n", stringify_result(result));
    }
    return 0;
  }

  // commands
  char *command = argv[1];
  if (starts_with(command, "run") && argc > 1) {
    char *file_path = argv[2];
    char *file_contents = read_filepath(file_path);

    Token *tokens = lex(file_contents);
    Node *program = create_node(NODE_PROGRAM, tokens, NULL);
    program->body = parse(tokens, NULL);
    Result *result = evaluate(program);
    stringify_result(result);

    return 0;
  }

  printf("Unknown command: %s", command);
  return 1;
}
