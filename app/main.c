#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Command {
  const char *name;
  const char *args;
};

int command_parse(char *input, struct Command *command);

int is_builtin(const char *name);

const size_t PATH_MAX = 4096;

struct EnvPath {
  char raw[4095];
  char *paths[100];
  size_t n_paths;
};

int env_path_parse(struct EnvPath *env_path, const char* input);
int env_path_find(struct EnvPath *env_path, const char* name, char *path);
int env_path_print(struct EnvPath *env_path);

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  // parse the PATH
  struct EnvPath env_path;
  if (env_path_parse(&env_path, getenv("PATH")) != 0) {
    fprintf(stderr, "failed to parse PATH\n");
    exit(1);
  }

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

    // type command
    if (strcmp(command.name, "type") == 0) {
      if (is_builtin(command.args)) {
        printf("%s is a shell builtin\n", command.args);
        continue;
      }

      char bin_path[4096];
      if (env_path_find(&env_path, command.args, bin_path) == 0) {
        printf("%s is %s\n", command.args, bin_path);
        continue;
      }

      printf("%s: not found\n", command.args);
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

int is_builtin(const char *name) {
  if (strcmp(name, "exit") == 0) {
    return 1;
  }
  if (strcmp(name, "echo") == 0) {
    return 1;
  }
  if (strcmp(name, "type") == 0) {
    return 1;
  }
  return 0;
}

int env_path_find(struct EnvPath *env_path, const char *name, char *path) {
  return 1;
}

int env_path_parse(struct EnvPath *env_path, const char *input) {
  strncpy(env_path->raw, input, PATH_MAX);
  char *token = strtok(env_path->raw, ":");
  size_t n_paths = 0;
  while (token != NULL && n_paths <= PATH_MAX) {
    env_path->paths[n_paths] = token;
    token = strtok(NULL, ":");
    n_paths++;
  }
  env_path->n_paths = n_paths;
  return 0;
}

int env_path_print(struct EnvPath *env_path) {
  for (size_t i = 0; i < env_path->n_paths; i++) {
    printf("%s\n", env_path->paths[i]);
  }
  return 0;
}
