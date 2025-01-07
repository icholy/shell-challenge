#include "../app/command.h"
#include "../app/env_path.h"
#include <check.h>
#include <stdlib.h>
#include <string.h>

START_TEST(env_path_parse_test) {
  struct EnvPath env_path;
  env_path_parse(&env_path, "/bin:/usr/bin");
  ck_assert_uint_eq(env_path.n_paths, 2);
  ck_assert_str_eq(env_path.paths[0], "/bin");
  ck_assert_str_eq(env_path.paths[1], "/usr/bin");
}
END_TEST

START_TEST(command_parse_simple) {
  struct Command command;
  char *input = strdup("cd /tmp");
  command_parse(&command, input);
  ck_assert_str_eq(command.name, "cd");
  ck_assert_str_eq(command_arg(&command, 0), "/tmp");
  free(input);
}
END_TEST

START_TEST(command_parse_single_quote) {
  struct Command command;
  char *input = strdup("echo 'hello world'");
  command_parse(&command, input);
  ck_assert_str_eq(command.name, "echo");
  ck_assert_str_eq(command_arg(&command, 0), "hello world");
  free(input);
}
END_TEST

Suite *test_suite(void) {
  Suite *s = suite_create("Shell");

  TCase *tc_env_path = tcase_create("EnvPath");
  tcase_add_test(tc_env_path, env_path_parse_test);
  suite_add_tcase(s, tc_env_path);

  TCase *tc_command = tcase_create("Command");
  tcase_add_test(tc_command, command_parse_simple);
  tcase_add_test(tc_command, command_parse_single_quote);
  suite_add_tcase(s, tc_command);

  return s;
}

int main(void) {
  Suite *s = test_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
