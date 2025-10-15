# RACE-UP-Assignment
RACE-UP Assignment task 0
I first created test_emergency file, where I integrated the Unity testing framework from Github (https://github.com/ThrowTheSwitch/Unity.git). I included the unity.h for test declarations and unity.c for test implementations; in my code I used employed Unity Macros (TEST_ASSERT_*, UNITY_BEGIN(), UNITY_END()) to write concise and readable texts.
In my file i created setUp() to call EmergencyNode_class_init(), resetting any global state before each test. 
I implmented an empty tearDown() placeholder for future cleanup needs. 
test_NodeInitialization verified that EmergencyNode_init resets emergency_counter to zero and clears the entire emergency_buffer.
