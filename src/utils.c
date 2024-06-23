#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string_duplicate(char *destination, size_t length) {
  if (destination == NULL) {
    destination = calloc(length + 1, sizeof(char));
  } else {
    destination = realloc(destination, length + 1);
  }

  if (destination == NULL) {
    printf("Can't malloc destination\n");
    exit(1);
  }

  return destination;
}

bool starts_with(const char *a, const char *b) {
  return strncmp(a, b, strlen(b)) == 0;
}

char *read_filepath(const char *filepath) {
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
  contents[file_size] = '\0';
  return contents;
}
