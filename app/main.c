#include <stdio.h>
#include <string.h>

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  // Uncomment this block to pass the first stage
  printf("$ ");

  // read line from stdin
  char *line = NULL;
  size_t line_alloc_size;
  size_t line_size = getline(&line, &line_alloc_size, stdin);
  if (line_size != -1) {
    line[line_size-1] = 0; // remove the trailing newline
    printf("%s: command not found\n", line);
  }

  // Wait for user input
  char input[100];
  fgets(input, 100, stdin);
  return 0;
}
