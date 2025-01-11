#ifndef SHELL_COMMAND_H
#define SHELL_COMMAND_H

#include <stddef.h>

#define SHELL_REDIRECT_STDOUT 1 << 0
#define SHELL_REDIRECT_STDERR 1 << 1
#define SHELL_REDIRECT_APPEND 1 << 2

struct Command {
  const char *name;
  char *argv[100];
  size_t narg;

  unsigned short flags;
  const char* redirect;
};

int command_parse(struct Command *command, char *input);
int command_print(struct Command *command);
const char *command_arg(struct Command *command, size_t n);

#endif /* SHELL_COMMAND_H */
