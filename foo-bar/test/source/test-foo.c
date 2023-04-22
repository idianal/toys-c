#include "foo.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(foo);

TEST_SETUP(foo)
{

}

TEST_TEAR_DOWN(foo)
{

}

TEST(foo, foo_shouldReturnString)
{
  TEST_ASSERT_EQUAL_STRING("foo", foo());
}
