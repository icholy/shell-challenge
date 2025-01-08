#include "command.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct CommandParser {
  char *next;
  char *input;
  char *output;
  char *arg;
};

int command_parser_init(struct CommandParser *parser, char *input) {
  parser->input = strdup(input);
  parser->next = parser->input;
  parser->output = input;
  parser->arg = NULL;
  return 0;
}

int command_parser_free(struct CommandParser *parser) {
  free(parser->input);
  return 0;
}

int is_space(char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\n':
    return 1;
  default:
    return 0;
  }
}

void command_parser_append(struct CommandParser *parser, char c) {
  parser->output[0] = c;
  parser->output++;
}

int command_parse_quote(struct CommandParser *parser) {
  // skip the opening quote
  parser->next++;
  if (parser->next[0] == 0) {
    return 1;
  }
  while (parser->next[0] != '\'' && parser->next[0] != 0) {
    command_parser_append(parser, parser->next[0]);
    parser->next++;
  }
  if (parser->next[0] == 0) {
    return 1;
  }
  parser->next++;
  return 0;
}

int command_parser_next(struct CommandParser *parser) {
  // skip whitespace
  while (is_space(parser->next[0])) {
    parser->next++;
  }
  // check for EOF
  if (parser->next[0] == 0) {
    parser->arg = NULL;
    return 0;
  }
  // remember the start of the next arg
  parser->arg = parser->output;
  // go forward until we hit a space or EOF
  while (!is_space(parser->next[0]) && parser->next[0] != 0) {
    // parse single quote
    if (parser->next[0] == '\'') {
      if (command_parse_quote(parser) != 0) {
        return 1;
      }
    } else {
      command_parser_append(parser, parser->next[0]);
      parser->next++;
    }
  }
  // null terminator
  command_parser_append(parser, 0);
  return 0;
}

/**
 * Parse a space separate command line.
 * This function modifies the input string.
 */
int command_parse(struct Command *command, char *input) {
  struct CommandParser parser;
  if (command_parser_init(&parser, input) != 0) {
    return 1;
  }
  command->name = input;
  command->narg = 0;
  while (1) {
    if (command_parser_next(&parser) != 0) {
      command_parser_free(&parser);
      return 1;
    }
    command->argv[command->narg] = parser.arg;
    if (parser.arg == NULL) {
      break;
    }
    command->narg++;
  }
  return command_parser_free(&parser);
}

int command_print(struct Command *command) {
  printf("Command: %s\n", command->name);
  printf("Args:\n");
  for (size_t i = 1; i < command->narg; i++) {
    printf("- %s\n", command->argv[i]);
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
