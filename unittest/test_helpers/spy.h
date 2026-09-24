#ifndef _SPY_H_
#define _SPY_H_

#include "unity.h"
#include <string.h>

#define SPY_MAX_CALLS 10

#define DECLARE_SPY(name, args_type) \
    struct { int calls; args_type history[SPY_MAX_CALLS]; } name

#define SPY_RECORD(name, args_value) do { \
    if ((name).calls < SPY_MAX_CALLS) { \
        (name).history[(name).calls] = (args_value); \
    } \
    (name).calls++; \
} while (0)

#define SPY_RESET(name) memset(&(name), 0, sizeof(name))

#define ASSERT_SPY_CALLED(name, n) TEST_ASSERT_EQUAL_INT((n), (name).calls)
#define ASSERT_SPY_NOT_CALLED(name) TEST_ASSERT_EQUAL_INT(0, (name).calls)

#endif
