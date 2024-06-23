#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  int length;
  char *value;
} String;

char *string_duplicate(char *destination, size_t length);
bool starts_with(const char *a, const char *b);
char *read_filepath(const char *filepath);
#endif
