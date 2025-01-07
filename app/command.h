#include <stddef.h>

struct Command {
  char *name;
  char *argv[100];
  size_t narg;
};

int command_parse(struct Command *command, char *input);
int command_print(struct Command *command);
const char *command_arg(struct Command *command, size_t n);
