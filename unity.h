#ifndef UNITY_H
#define UNITY_H

#include <stdio.h>

void UnityBegin(const char* filename);
int UnityEnd(void);
void UnityAssertEqualInt(int expected, int actual, const char* msg, int line);
void UnityRunTest(void (*test_func)(void), const char* test_name);

#define UNITY_BEGIN() UnityBegin(__FILE__)
#define UNITY_END() UnityEnd()
#define RUN_TEST(func) UnityRunTest(func, #func)
#define TEST_ASSERT_EQUAL_INT(expected, actual) UnityAssertEqualInt(expected, actual, #expected, __LINE__)
#define TEST_ASSERT_TRUE(condition) UnityAssertEqualInt(1, condition, #condition, __LINE__)
#define TEST_ASSERT_FALSE(condition) UnityAssertEqualInt(0, condition, #condition, __LINE__)

#endif