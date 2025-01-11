#include "command.h"
#include "env_path.h"
#include "shared.h"
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const char *gethomedir() {
  const char *dir = getenv("HOME");
  if (dir == NULL) {
    dir = getpwuid(getuid())->pw_dir;
  }
  return dir;
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
  if (strcmp(name, "pwd") == 0) {
    return 1;
  }
  if (strcmp(name, "cd") == 0) {
    return 1;
  }
  return 0;
}

int execute_command(struct Command *command, struct EnvPath *env_path) {

  // exit command
  if (strcmp(command->name, "exit") == 0) {
    exit(0);
  }

  // echo command
  if (strcmp(command->name, "echo") == 0) {
    for (size_t i = 0; i < command->narg - 1; i++) {
      if (i > 0) {
        printf(" ");
      }
      printf("%s", command_arg(command, i));
    }
    printf("\n");
    return 0;
  }

  // debug command
  if (strcmp(command->name, "debug") == 0) {
    command_print(command);
    return 0;
  }

  // type command
  if (strcmp(command->name, "type") == 0) {
    if (is_builtin(command_arg(command, 0))) {
      printf("%s is a shell builtin\n", command_arg(command, 0));
      return 0;
    }

    char bin_path[4096];
    if (env_path_find(env_path, command_arg(command, 0), bin_path) == 0) {
      printf("%s is %s\n", command_arg(command, 0), bin_path);
      return 0;
    }

    printf("%s: not found\n", command_arg(command, 0));
    return 0;
  }

  // cwd command
  if (strcmp(command->name, "pwd") == 0) {
    char buf[PATH_MAX];
    char *cwd = getcwd(buf, PATH_MAX);
    printf("%s\n", cwd);
    return 0;
  }

  // cd command
  if (strcmp(command->name, "cd") == 0) {
    const char *dir = command_arg(command, 0);
    if (strcmp(dir, "~") == 0) {
      dir = gethomedir();
    }
    if (chdir(dir) != 0) {
      printf("cd: %s: No such file or directory\n", dir);
    }
    return 0;
  }

  char bin_path[4096];
  if (env_path_find(env_path, command->name, bin_path) == 0) {
    pid_t child = fork();
    if (child) {
      waitpid(child, NULL, 0);
    } else {
      if (command->redirect != NULL) {
        int fd = open(command->redirect, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd == -1) {
          printf("failed to open file: %s\n", command->redirect);
          return 1;
        }
        if (dup2(fd, 1) < 0) {
          printf("failed to redirect stdout: %s\n", command->redirect);
          return 1;
        }
      }
      if (execve(bin_path, command->argv, NULL) != 0) {
        printf("failed to execute: %s\n", command->name);
        return 1;
      }
    }
    return 0;
  }

  // print error
  printf("%s: command not found\n", command->name);
  return 0;
}

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

    // execute the command
    if (execute_command(&command, &env_path) != 0) {
      return 0;
    }
  }

  return 0;
}
