#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int str_starts_with(char *string, char *starts_with) {
  return strncmp(string, starts_with, strlen(starts_with)) == 0;
}

char *read_filepath(char *filepath) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    printf("Error: Filepath %s does not exist.\n", filepath);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);

  char *contents = malloc(file_size + 1);
  if (contents == NULL) {
    printf("No malloc :(\n");
    exit(1);
  }

  rewind(file);
  fread(contents, file_size, 1, file);
  fclose(file);

  // null-terminate
  contents[file_size] = 0;
  return contents;
}