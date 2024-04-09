#include <stdio.h>

int main(int argc, char *argv[]) {
  char *test = "hi";
  // just testing how indexing char* and formatting strings works
  printf("%d, %d", test[1] == 'i', test[0]);
  return 0;
}
