#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  // Uncomment this block to pass the first stage
  printf("$ ");

  // Wait for user input
  char input[100];
  fgets(input, 100, stdin);

  // remove trailing newline
  char *nl_index = strstr(input, "\n");
  if (nl_index != NULL) {
    *nl_index = 0;
  }

  // print error
  printf("%s: command not found\n", input);

  return 0;
}
