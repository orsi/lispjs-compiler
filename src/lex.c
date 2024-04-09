#include <stdio.h>
#include <string.h>
enum TokenType { Symbol, Literal, Keyword };
typedef struct {
  enum TokenType type;
  char *value;
  int column;
  int line;
} Token;

Token *lex(char *input) {
  char *current = input;
  Token *tokens = {0};
  while (*current) {
    switch (*current) {
    case '\0':
      printf("null\n");
      break;
    case ' ':
      printf("space\n");
      break;
    // all symbols on typical English QWERTY keyboard
    case '`':
      printf("`\n");
      break;
    case '~':
      printf("~\n");
      break;
    case '!':
      printf("!\n");
      break;
    case '@':
      printf("@\n");
      break;
    case '#':
      printf("#\n");
      break;
    case '$':
      printf("$\n");
      break;
    case '%':
      printf("%%\n");
      break;
    case '^':
      printf("^\n");
      break;
    case '&':
      printf("&\n");
      break;
    case '*':
      printf("*\n");
      break;
    case '(':
      printf("(\n");
      break;
    case ')':
      printf(")\n");
      break;
    case '-':
      printf("-\n");
      break;
    case '_':
      printf("_\n");
      break;
    case '=':
      printf("=\n");
      break;
    case '+':
      printf("+\n");
      break;
    case '[':
      printf("[\n");
      break;
    case '{':
      printf("{\n");
      break;
    case ']':
      printf("]\n");
      break;
    case '}':
      printf("}\n");
      break;
    case '\\':
      printf("\\\n");
      break;
    case '|':
      printf("|\n");
      break;
    case ';':
      printf(";\n");
      break;
    case ':':
      printf(":\n");
      break;
    case '\'':
      printf("'\n");
      break;
    case '"':
      printf("\"\n");
      break;
    case ',':
      printf(",\n");
      break;
    case '<':
      printf("<\n");
      break;
    case '.':
      printf(".\n");
      break;
    case '>':
      printf(">\n");
      break;
    case '/':
      printf("/\n");
      break;
    case '?':
      printf("?\n");
      break;
    // non-printable characters
    case '\n':
      printf("newline\n");
      break;
    default:
      if (*current >= '0' || *current <= '9') {
        char *start = current;
        while (*current != '\0' && ((*current >= '0' && *current <= '9') ||
                                    (*current == '.' && (current[1] >= '0' &&
                                                         current[1] <= '9')))) {
          current++;
        }
        int size = current - start;
        printf("%.*s\n", size, start);
      } else {
        printf("unknown: %c\n", *current);
      }
    }

    current++;
  }

  return tokens;
}