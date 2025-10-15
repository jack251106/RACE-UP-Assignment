# RACE-UP-Assignment

RACE-UP Assignment task 0. 

I first created test_emergency file, where I integrated the Unity testing framework from Github (https://github.com/ThrowTheSwitch/Unity.git). I included the unity.h for test declarations and unity.c for test implementations; in my code I used employed Unity Macros (TEST_ASSERT_*, UNITY_BEGIN(), UNITY_END()) to write concise and readable texts. 
In my file i created setUp() to call EmergencyNode_class_init(), resetting any global state before each test.  
I implmented an empty tearDown() placeholder for future cleanup needs. 
test_NodeInitialization verified that EmergencyNode_init resets emergency_counter to zero and clears the entire emergency_buffer.
I used TEST_ASSERT_EQUAL_INT(0, ...) for both the counter and each buffer byte, to confirm a clean initial state. 
To raise a specific bit and to assert that the correct bit mask appeared in the buffer I used test_RaiseEmergency; then i utilized test_SolveEmergency to raise and solve the same bit, asserting the bit was cleared and the counter decremented back to zero. 
The funtcion test_EmergencyStateCheck invoked EmergencyNode_is_emergency_state before and after raising a bit, verifying inactive (O) then active (non-zero) states.
tet_DestructionEmergencyNode raised EmergencyNode_destroy, then asserted that both buffer and counter returned to zero.
I also used a Guard testing for invalid inputs. Then I enforced inter-emergency dependencies, where bit 4 depends on bit 2. 
At the end I did a performance testing, where I meausred the time to execute 1000 raise operations in a loop, the time to solve the first raised 1000 emergencies, and then perfomed 10000 mixed raise/solve operations and recorded timing metrics. 
