#include "command.h"
#include <stdio.h>
#include <string.h>

/**
 * Parse a space separate command line.
 * This function modifies the input string.
 */
int command_parse(struct Command *command, char *input) {
  // remove trailing newline
  char *end = strchr(input, '\n');
  if (end != NULL) {
    *end = 0;
  }

  size_t narg = 0;
  char *token = strtok(input, " ");

  command->name = token;

  while (token != NULL) {
    command->argv[narg] = token;
    narg++;
    token = strtok(NULL, " ");
  }
  command->argv[narg] = NULL;

  command->name = input;
  command->narg = narg;

  return 0;
}

int command_print(struct Command *command) {
  printf("Command: %s\n", command->name);
  printf("Args:\n");
  for (size_t i = 1; i < command->narg; i++) {
    printf("- %s\n", command->argv[i]);
  }
  return 0;
}

const char *command_arg(struct Command *command, size_t n) {
  n++;
  if (n >= command->narg || n <= 0) {
    return "";
  }
  return command->argv[n];
}
