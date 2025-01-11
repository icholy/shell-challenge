#include "../app/args.h"
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

START_TEST(args_parse_simple) {
  struct Args args;
  char *input = strdup("cd /tmp");
  args_parse(&args, input);
  ck_assert_str_eq(args_nth(&args, 0), "cd");
  ck_assert_str_eq(args_nth(&args, 1), "/tmp");
  free(input);
}
END_TEST

START_TEST(args_parse_single_quote) {
  struct Args args;
  char *input = strdup("echo 'script     example' 'world''test'");
  args_parse(&args, input);
  ck_assert_int_eq(args.narg, 3);
  ck_assert_str_eq(args_nth(&args, 0), "echo");
  ck_assert_str_eq(args_nth(&args, 1), "script     example");
  ck_assert_str_eq(args_nth(&args, 2), "worldtest");
  free(input);
}
END_TEST

START_TEST(args_parse_double_quote) {
  struct Args args;
  char *input = strdup("echo \"hello    world\" \"another\"\"test\"");
  args_parse(&args, input);
  ck_assert_int_eq(args.narg, 3);
  ck_assert_str_eq(args_nth(&args, 0), "echo");
  ck_assert_str_eq(args_nth(&args, 1), "hello    world");
  ck_assert_str_eq(args_nth(&args, 2), "anothertest");
  free(input);
}
END_TEST

START_TEST(args_parse_escaped_chars_unquoted) {
  struct Args args;
  char *input = strdup("echo hello\\ world file\\\"name file\\\'s");
  args_parse(&args, input);
  ck_assert_int_eq(args.narg, 4);
  ck_assert_str_eq(args_nth(&args, 0), "echo");
  ck_assert_str_eq(args_nth(&args, 1), "hello world");
  ck_assert_str_eq(args_nth(&args, 2), "file\"name");
  ck_assert_str_eq(args_nth(&args, 3), "file's");
  free(input);
}
END_TEST

START_TEST(args_parse_escaped_chars_quoted) {
  struct Args args;
  char *input = strdup("echo \"hello\\\"world\" 'tests\\\"quoted\\\"'");
  args_parse(&args, input);
  ck_assert_int_eq(args.narg, 3);
  ck_assert_str_eq(args_nth(&args, 0), "echo");
  ck_assert_str_eq(args_nth(&args, 1), "hello\"world");
  ck_assert_str_eq(args_nth(&args, 2), "tests\\\"quoted\\\"");
  free(input);
}
END_TEST

START_TEST(args_parse_special_paths) {
  struct Args args;
  char *input = strdup("cat \"/tmp/quz/f\\n64\" \"/tmp/quz/f\\7\" \"/tmp/quz/f'\\\'19\"");
  args_parse(&args, input);
  ck_assert_int_eq(args.narg, 4);
  ck_assert_str_eq(args_nth(&args, 0), "cat");
  ck_assert_str_eq(args_nth(&args, 1), "/tmp/quz/f\\n64");
  ck_assert_str_eq(args_nth(&args, 2), "/tmp/quz/f\\7");
  ck_assert_str_eq(args_nth(&args, 3), "/tmp/quz/f'\\'19");
  free(input);
}
END_TEST

START_TEST(args_parse_mixed_quotes_escape) {
  struct Args args;
  char *input = strdup("echo \"world'test'\\n'shell\" \"example'hello'\\\\n'shell\"");
  args_parse(&args, input);
  ck_assert_int_eq(args.narg, 3);
  ck_assert_str_eq(args_nth(&args, 0), "echo");
  ck_assert_str_eq(args_nth(&args, 1), "world'test'\\n'shell");
  ck_assert_str_eq(args_nth(&args, 2), "example'hello'\\n'shell");
  free(input);
}
END_TEST

Suite *test_suite(void) {
  Suite *s = suite_create("Shell");

  TCase *tc_env_path = tcase_create("EnvPath");
  tcase_add_test(tc_env_path, env_path_parse_test);
  suite_add_tcase(s, tc_env_path);

  TCase *tc_args = tcase_create("args");
  tcase_add_test(tc_args, args_parse_simple);
  tcase_add_test(tc_args, args_parse_single_quote);
  tcase_add_test(tc_args, args_parse_double_quote);
  tcase_add_test(tc_args, args_parse_escaped_chars_unquoted);
  tcase_add_test(tc_args, args_parse_escaped_chars_quoted);
  tcase_add_test(tc_args, args_parse_special_paths);
  tcase_add_test(tc_args, args_parse_mixed_quotes_escape);
  suite_add_tcase(s, tc_args);

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
