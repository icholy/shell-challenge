#include "env_path.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/limits.h>

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
