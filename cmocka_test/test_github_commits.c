#include <string.h>
#include <stdlib.h>

#include "github_commits_mod.h"
#include "test_github_commits.h"

char * __wrap_get_result(const char *user, const char *repository) {
	check_expected_ptr(user);
	check_expected_ptr(repository);
	return mock_ptr_type(char *);
}

static void test_successful_get_first(void **state) {
	(void) state;
	expect_string(__wrap_get_result, user, "AdamK90");
	expect_string(__wrap_get_result, repository, "training-project-lab");

	const char *cjson = "[{\"sha\": \"2a779ce866d8207a4706e9c54d3e0032f6ee2821\",\"commit\":{\"message\":\"Add files via upload\"}}]";
	char *json = (char*)malloc(strlen(cjson) + 1);
	strcpy(json, cjson);
	will_return(__wrap_get_result, json);

	int error;
	commit_data d = get_first("AdamK90", "training-project-lab", &error);

	assert_string_equal(d.sha, "2a779ce866d8207a4706e9c54d3e0032f6ee2821");
	assert_string_equal(d.message, "Add files via upload");
	assert_int_equal(error, 0);
}

static void test_root_not_array(void **state) {
	(void) state;
	expect_string(__wrap_get_result, user, "AdamK90");
	expect_string(__wrap_get_result, repository, "training-project-lab");

	const char *cjson = "{\"sha\": \"2a779ce866d8207a4706e9c54d3e0032f6ee2821\",\"commit\":{\"message\":\"Add files via upload\"}}";
	char *json = (char*)malloc(strlen(cjson) + 1);
	strcpy(json, cjson);
	will_return(__wrap_get_result, json);

	int error;
	commit_data d = get_first("AdamK90", "training-project-lab", &error);
	assert_null(d.sha);
	assert_null(d.message);
	assert_int_equal(error, 2);
}

static void test_first_not_object(void **state) {
	(void) state;
	expect_string(__wrap_get_result, user, "AdamK90");
	expect_string(__wrap_get_result, repository, "training-project-lab");

	const char *cjson = "[\"sha\", \"message\"]";
	char *json = (char*)malloc(strlen(cjson) + 1);
	strcpy(json, cjson);
	will_return(__wrap_get_result, json);

	int error;
	commit_data d = get_first("AdamK90", "training-project-lab", &error);
	assert_null(d.sha);
	assert_null(d.message);
	assert_int_equal(error, 3);
}

int main() {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_successful_get_first),
		cmocka_unit_test(test_root_not_array),
		cmocka_unit_test(test_first_not_object),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}