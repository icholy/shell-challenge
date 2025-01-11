#ifndef SHELL_ENV_PATH_H
#define SHELL_ENV_PATH_H

#include <stddef.h>

struct EnvPath {
  char raw[4095];
  char *paths[100];
  size_t n_paths;
};

int env_path_parse(struct EnvPath *env_path, const char *input);
int env_path_find(struct EnvPath *env_path, const char *name, char *path);
int env_path_print(struct EnvPath *env_path);

#endif /* SHELL_ENV_PATH_H */
