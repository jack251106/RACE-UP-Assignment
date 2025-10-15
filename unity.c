#include "unity.h"

static int tests_run = 0;
static int tests_failed = 0;

void UnityBegin(const char* filename) {
    tests_run = 0;
    tests_failed = 0;
    printf("==== Running tests in %s ====\n", filename);
}

int UnityEnd(void) {
    printf("==== Tests run: %d, Failures: %d ====\n", tests_run, tests_failed);
    return tests_failed;
}

void UnityAssertEqualInt(int expected, int actual, const char* msg, int line) {
    tests_run++;
    if (expected != actual) {
        tests_failed++;
        printf("Failure: %s (line %d): Expected %d but got %d\n", msg, line, expected, actual);
    }
}

void UnityRunTest(void (*test_func)(void), const char* test_name) {
    printf("Running test: %s\n", test_name);
    test_func();
}