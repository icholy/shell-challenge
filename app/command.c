#include "command.h"
#include <stdio.h>
#include <string.h>

struct CommandParser {
  char *input;
  size_t offset;
  char next;
  char *arg;
};

int command_parser_init(struct CommandParser *parser, char *input) {
  parser->offset = 0;
  parser->input = input;
  parser->arg = NULL;
  parser->next = input[0];
  return 0;
}

int is_digit(char c) { return '0' <= c && c <= '9'; }

int is_alpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
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

int is_ident(char c) {
  if (is_space(c)) {
    return 0;
  }
  return is_digit(c) || is_alpha(c);
}

void command_parser_inc(struct CommandParser *parser) {
  parser->offset++;
  parser->next = parser->input[parser->offset];
}

int command_parser_next(struct CommandParser *parser) {
  // skip whitespace
  while (is_space(parser->next)) {
    command_parser_inc(parser);
  }
  // check for EOF
  if (parser->next == 0) {
    parser->arg = NULL;
    return 0;
  }
  // parse single quote
  if (parser->next == '\'') {
    command_parser_inc(parser);
    if (parser->next == 0) {
      return 1;
    }
    parser->arg = parser->input + parser->offset;
    while (parser->next != '\'' && parser->next != 0) {
      command_parser_inc(parser);
    }
    if (parser->next == 0) {
      return 1;
    }
    parser->input[parser->offset] = 0;
    command_parser_inc(parser);
  } else {
    // remember the start of the next arg
    parser->arg = parser->input + parser->offset;
    // go forward until we hit a space or EOF
    while (!is_space(parser->next) && parser->next != 0) {
      command_parser_inc(parser);
    }
    // add the null terminator
    parser->input[parser->offset] = 0;
  }
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
      return 1;
    }
    command->argv[command->narg] = parser.arg;
    if (parser.arg == NULL) {
      break;
    }
    command->narg++;
  }
  return 0;
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
