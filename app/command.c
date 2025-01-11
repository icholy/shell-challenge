#include "command.h"
#include "args.h"
#include <stdio.h>
#include <string.h>

int command_parse(struct Command *command, char *input) {
  struct Args args;
  if (args_parse(&args, input) != 0) {
    return 1;
  }
  command->name = args_nth(&args, 0);
  command->redirect = NULL;
  command->flags = 0;
  command->narg = 0;
  for (size_t i = 0; i < args.narg; i++) {
    if (strcmp(args.argv[i], "1>") == 0 || strcmp(args.argv[i], ">") == 0) {
      command->redirect = args.argv[i + 1];
      command->flags = SHELL_REDIRECT_STDOUT;
      break;
    }
    if (strcmp(args.argv[i], "1>>") == 0 || strcmp(args.argv[i], ">>") == 0) {
      command->redirect = args.argv[i + 1];
      command->flags = SHELL_REDIRECT_STDOUT | SHELL_REDIRECT_APPEND;
      break;
    }
    if (strcmp(args.argv[i], "2>") == 0) {
      command->redirect = args.argv[i + 1];
      command->flags = SHELL_REDIRECT_STDERR;
      break;
    }
    if (strcmp(args.argv[i], "2>>") == 0) {
      command->redirect = args.argv[i + 1];
      command->flags = SHELL_REDIRECT_STDERR | SHELL_REDIRECT_APPEND;
      break;
    }
    command->argv[i] = args.argv[i];
    command->narg++;
  }
  command->argv[command->narg] = NULL;
  return 0;
}

int command_print(struct Command *command) {
  printf("Args:\n");
  for (size_t i = 0; i < command->narg; i++) {
    printf("- %s\n", command->argv[i]);
  }
  printf("Flags:\n");
  if (command->flags & SHELL_REDIRECT_STDOUT) {
    printf("- SHELL_REDIRECT_STDOUT\n");
  }
  if (command->flags & SHELL_REDIRECT_STDERR) {
    printf("- SHELL_REDIRECT_STDERR\n");
  }
  if (command->flags & SHELL_REDIRECT_APPEND) {
    printf("- SHELL_REDIRECT_APPEND\n");
  }
  if (command->redirect != NULL) {
    printf("Redirect: %s\n", command->redirect);
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
