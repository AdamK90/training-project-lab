#ifndef MOCK_GET_RESULT_H
#define MOCK_GET_RESULT_H
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

char * __wrap_get_result(const char *user, const char *repository);

#endif
