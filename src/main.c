#include <stdio.h>
#include <sys/syslimits.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("arg count %d\n", argc);

  for (size_t i = 0; i < argc; i++) {
    printf("%d: %s\n", (int)i, argv[i]);
  }

  FILE *file;
  char ch;

  char cwd[PATH_MAX];
  printf("current dir: %s\n", getcwd(cwd, sizeof(cwd)));

  file = fopen(argv[1], "r");
  if (file == NULL) {
    printf("Bad file: \"%s\"\n", argv[1]);
  }

  printf("Reading file \"%s\"\n", argv[1]);
  printf("-----\n");

  ch = fgetc(file);
  while (ch != EOF) {
    printf("%c", ch);
    ch = fgetc(file);
  }

  printf("\n-----");

  fclose(file);

  // beep end
  printf("\a");
  return 0;
}