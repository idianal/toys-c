#include "bar.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(bar);

TEST_SETUP(bar) {

}

TEST_TEAR_DOWN(bar) {

}

TEST(bar, bar_shouldReturnString) {
  TEST_ASSERT_EQUAL_STRING("bar", bar());
}
