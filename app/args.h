#ifndef SHELL_ARGS_H
#define SHELL_ARGS_H

#include <stddef.h>

struct Args {
  char *argv[100];
  size_t narg;
};

int args_parse(struct Args *args, char *input);
int args_print(struct Args *args);
const char *args_nth(struct Args *args, size_t n);

#endif /* SHELL_ARGS_H */
