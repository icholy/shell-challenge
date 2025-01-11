#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

#include <stddef.h>

struct Command {
  const char *name;
  char *argv[100];
  size_t narg;
};

int command_parse(struct Command *command, char *input);
int command_print(struct Command *command);
const char *command_arg(struct Command *command, size_t n);

#endif /* SHELL_COMMAND_H */
