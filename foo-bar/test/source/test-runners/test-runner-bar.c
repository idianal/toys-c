#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(bar)
{
    RUN_TEST_CASE(bar, bar_shouldReturnString);
}
