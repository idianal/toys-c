#include "unity_fixture.h"

static void run(void) {
    RUN_TEST_GROUP(foo);
    RUN_TEST_GROUP(bar);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, run);
}
