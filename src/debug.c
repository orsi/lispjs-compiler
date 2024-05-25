#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  printf("wtf does strcmp do? %d\n", strcmp("?!.", "?!"));

  const char *hi = "hi";
  // just testing how indexing char* and formatting strings works
  printf("%d, %d", hi[1] == 'i', hi[0]);

  // checking out malloc, pointer addressing instead of array[0] addressing, and
  // what happens when you access memory outside of malloc allocation
  int length = 5;
  int *myArray = malloc(length * sizeof(int));
  int *start = myArray;

  int count = 100; // will go beyond end of allocated memory!
  for (int i = 0; i < count; i++) {
    *myArray = i;
    myArray++;
  }

  // but no seg fault...? maybe -- depends on how far past we go

  // ...but, now that test is pointing somewhere else...
  // free(test);  --> would error!

  //  debug(24077,0x1d99b9000)
  //  malloc: *** error for object 0x600001bb53f0: pointer being freed was not
  //  allocated debug(24077,0x1d99b9000)
  //  malloc: *** set a breakpoint in
  //  malloc_error_break to debug make: *** [debug] Abort trap: 6

  free(start); // no error - original address of myArray
  return 0;
}
