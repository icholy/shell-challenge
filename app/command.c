#include "command.h"
#include "args.h"
#include <stdio.h>

int command_parse(struct Command *command, char *input) {
  struct Args args;
  if (args_parse(&args, input) != 0) {
    return 1;
  }
  command->name = args_nth(&args, 0);
  command->narg = args.narg;
  *command->argv = *args.argv;
  return 0;
}

int command_print(struct Command *command) {
  printf("Args: %s\n", command->name);
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
