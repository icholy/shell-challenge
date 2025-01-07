#include "../app/env_path.h"
#include <check.h>
#include <stdlib.h>

START_TEST (env_path_parse_test)
{
	struct EnvPath env_path;
	env_path_parse(&env_path, "/bin:/usr/bin");
	ck_assert_uint_eq(env_path.n_paths, 2);
	ck_assert_str_eq(env_path.paths[0], "/bin");
	ck_assert_str_eq(env_path.paths[1], "/usr/bin");
}
END_TEST

Suite * test_suite(void)
{
	Suite *s;
	TCase *tc_env_path;

	s = suite_create("Shell");

	tc_env_path = tcase_create("EnvPath");
	tcase_add_test(tc_env_path, env_path_parse_test);
	suite_add_tcase(s, tc_env_path);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = test_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

