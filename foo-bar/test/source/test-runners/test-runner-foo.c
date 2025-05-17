#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(foo) {
    RUN_TEST_CASE(foo, foo_shouldReturnString);
}
