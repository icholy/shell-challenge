#include "args.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ArgsParser {
  char *next;
  char *input;
  char *output;
  char *arg;
};

int args_parser_init(struct ArgsParser *parser, char *input) {
  parser->input = strdup(input);
  parser->next = parser->input;
  parser->output = input;
  parser->arg = NULL;
  return 0;
}

int args_parser_free(struct ArgsParser *parser) {
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

void args_parser_append(struct ArgsParser *parser, char c) {
  parser->output[0] = c;
  parser->output++;
}

bool is_digit(char c) { return c >= '0' && c <= '9'; }

int args_parser_escaped(struct ArgsParser *parser, bool double_quote) {
  parser->next++; // skip the forward slash
  if (parser->next[0] == 0) {
    return 1;
  }
  if (!double_quote || parser->next[0] != '"') {
    args_parser_append(parser, '\\');
  }
  args_parser_append(parser, '"');
  return 0;
}

int args_parse_quote(struct ArgsParser *parser) {
  char quote = parser->next[0];
  // skip the opening quote
  parser->next++;
  if (parser->next[0] == 0) {
    return 1;
  }
  while (parser->next[0] != quote && parser->next[0] != 0) {
    // handle escaping
    if (parser->next[0] == '\\') {
      parser->next++;
      if (parser->next[0] == 0) {
        return 1;
      }
      if (quote != '"' || (parser->next[0] != '"' && parser->next[0] != '$' && parser->next[0] != '\\')) {
        args_parser_append(parser, '\\');
      }
      args_parser_append(parser, parser->next[0]);
      parser->next++;
      continue;
    }
    args_parser_append(parser, parser->next[0]);
    parser->next++;
  }
  if (parser->next[0] == 0) {
    return 1;
  }
  parser->next++;
  return 0;
}

int args_parser_next(struct ArgsParser *parser) {
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
    // handle escaping
    if (parser->next[0] == '\\') {
      parser->next++;
      if (parser->next[0] == 0) {
        return 1;
      }
      args_parser_append(parser, parser->next[0]);
      parser->next++;
      continue;
    }
    // parse quoted string
    if (parser->next[0] == '\'' || parser->next[0] == '"') {
      if (args_parse_quote(parser) != 0) {
        return 1;
      }
      continue;
    }
    args_parser_append(parser, parser->next[0]);
    parser->next++;
  }
  // null terminator
  args_parser_append(parser, 0);
  return 0;
}

/**
 * Parse a space separate args line.
 * This function modifies the input string.
 */
int args_parse(struct Args *args, char *input) {
  struct ArgsParser parser;
  if (args_parser_init(&parser, input) != 0) {
    return 1;
  }
  args->narg = 0;
  while (1) {
    if (args_parser_next(&parser) != 0) {
      args_parser_free(&parser);
      return 1;
    }
    args->argv[args->narg] = parser.arg;
    if (parser.arg == NULL) {
      break;
    }
    args->narg++;
  }
  return args_parser_free(&parser);
}

int args_print(struct Args *args) {
  printf("Args:\n");
  for (size_t i = 0; i < args->narg; i++) {
    printf("- %s\n", args->argv[i]);
  }
  return 0;
}

const char *args_nth(struct Args *args, size_t n) {
  if (n >= args->narg) {
    return "";
  }
  return args->argv[n];
}
