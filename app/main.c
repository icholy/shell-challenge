#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Command {
  const char* name;
  const char* args;
};

int command_parse(char *input, struct Command *command);

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  while (1) {
    // Uncomment this block to pass the first stage
    printf("$ ");

    // Wait for user input
    char input[100];
    if (fgets(input, 100, stdin) == NULL) {
      break;
    }

    // parse the command
    struct Command command;
    if (command_parse(input, &command) != 0) {
      fprintf(stderr, "failed to parse command\n");
      exit(1);
    }

    // exit command
    if (strcmp(command.name, "exit") == 0) {
      exit(0);
    }

    // echo command
    if (strcmp(command.name, "echo") == 0) {
      puts(command.args);
      continue;
    }

    // print error
    printf("%s: command not found\n", command.name);
  }

  return 0;
}

/**
 * Parse a space separate command line.
 * This function modifies the input string.
 */
int command_parse(char *input, struct Command *command) {
  // remove trailing newline
  char *end = strchr(input, '\n');
  if (end != NULL) {
    *end = 0;
  }

  // replace space with null terminator
  char *space = strchr(input, ' ');
  if (space != NULL) {
    *space = 0;
  }

  // find the beginning of the args
  char *args;
  if (space != NULL) {
    args = space + 1;
  } else {
    args = input + strlen(input);
  }

  command->name = input;
  command->args = args;

  return 0;
}
