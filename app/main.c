#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

struct Command {
  char *name;
  char *args;
  char *argv[100];
  size_t narg;
};

int command_parse(struct Command *command, char *input);
const char *command_arg(struct Command *command, size_t n);

int is_builtin(const char *name);

const size_t PATH_MAX = 4096;

struct EnvPath {
  char raw[4095];
  char *paths[100];
  size_t n_paths;
};

int env_path_parse(struct EnvPath *env_path, const char *input);
int env_path_find(struct EnvPath *env_path, const char *name, char *path);
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
    if (command_parse(&command, input) != 0) {
      fprintf(stderr, "failed to parse command\n");
      continue;
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
      if (is_builtin(command_arg(&command, 0))) {
        printf("%s is a shell builtin\n", command.args);
        continue;
      }

      char bin_path[4096];
      if (env_path_find(&env_path, command_arg(&command, 0), bin_path) == 0) {
        printf("%s is %s\n", command.args, bin_path);
        continue;
      }

      printf("%s: not found\n", command.args);
      continue;
    }

    char bin_path[4096];
    if (env_path_find(&env_path, command.name, bin_path) == 0) {
      pid_t child = fork();
      if (child) {
        waitpid(child, NULL, 0);
      }
      execve(bin_path, command.argv, NULL);
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
int command_parse(struct Command *command, char *input) {
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

  // tokenize the args
  size_t narg = 0;
  char *token = strtok(args, " ");
  while (token != NULL) {
    command->argv[narg] = token;
    narg++;
    token = strtok(NULL, " ");
  }
  command->argv[narg] = NULL;

  command->name = input;
  command->args = args;
  command->narg = narg;

  return 0;
}

const char *command_arg(struct Command *command, size_t n) {
  if (n >= command->narg || n < 0) {
    return "";
  }
  return command->argv[n];
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

int env_path_find(struct EnvPath *env_path, const char *name, char *dest) {
  for (size_t i = 0; i < env_path->n_paths; i++) {
    strcpy(dest, env_path->paths[i]);
    size_t len = strlen(dest);
    if (dest[len - 1] != '/') {
      dest[len] = '/';
      dest[len + 1] = 0;
    }
    strcat(dest, name);
    struct stat tmp;
    if (stat(dest, &tmp) == 0) {
      return 0;
    }
  }
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
