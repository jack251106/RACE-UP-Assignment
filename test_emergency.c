#include "unity.h"
#include "emergency_module.h"
#include <time.h>       // For time measurement functions
#include <sys/time.h>   // For more precise time measurement


// Define dependencies for specific emergencies
// For example: emergency 4 depends on emergency 2
static const uint8_t emergency_dependencies[NUM_EMERGENCY_BUFFER * 8] = {
    [4] = 1 << 2,  // emergency 4 depends on emergency 2 (bit 2)
};

// Helper function to check if the emergency can be solved (no active dependencies)
int can_solve(const EmergencyNode_t* node, uint8_t exception) {
    uint8_t deps = emergency_dependencies[exception];
    // Check each bit dependency
    for (uint8_t bit = 0; bit < NUM_EMERGENCY_BUFFER * 8; bit++) {
        if ((deps & (1 << bit)) && (node->emergency_buffer[bit / 8] & (1 << (bit % 8)))) {
            return 0;  // Dependency still active, cannot resolve
        }
    }
    return 1;  // All dependencies are cleared
}

// Setup runs before each test
void setUp(void) {
    EmergencyNode_class_init();  // Reset global state
}

// This function gets the current time in microseconds
double get_time_microseconds(void) {
    struct timeval tv;                    // Create a structure to hold time
    gettimeofday(&tv, NULL);             // Get the current time
    return tv.tv_sec * 1000000.0 + tv.tv_usec;  // Convert to microseconds
}

// Test for initial reset
void test_NodeInitialization(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);
    TEST_ASSERT_EQUAL_INT(0, node.emergency_counter);
    for (int i = 0; i < NUM_EMERGENCY_BUFFER; i++) {
        TEST_ASSERT_EQUAL_INT(0, node.emergency_buffer[i]);
    }
}

// Test raising an emergency
void test_RaiseEmergency(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);
    int ret = EmergencyNode_raise(&node, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);
    // Buffer should have bit 3 set => value 8
    TEST_ASSERT_EQUAL_INT(8, node.emergency_buffer[0] & (1 << 3));
}

// Test solving an emergency
void test_SolveEmergency(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);
    EmergencyNode_raise(&node, 3);
    int ret = EmergencyNode_solve(&node, 3);
    TEST_ASSERT_EQUAL_INT(0, ret);
    // Bit should now be cleared
    TEST_ASSERT_EQUAL_INT(0, node.emergency_buffer[0] & (1 << 3));
    TEST_ASSERT_EQUAL_INT(0, node.emergency_counter);
}

// Test emergency active state
void test_EmergencyStateCheck(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);
    // Ensure fresh state
    EmergencyNode_class_init();

    // Should be inactive after init
    TEST_ASSERT_EQUAL_INT(0, EmergencyNode_is_emergency_state(&node));
    // Raise emergency
    EmergencyNode_raise(&node, 2);
    // State should be active
    TEST_ASSERT_TRUE(EmergencyNode_is_emergency_state(&node));
}

// Test destruction resets
void test_DestroyEmergencyNode(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);
    EmergencyNode_raise(&node, 5);
    int ret = EmergencyNode_destroy(&node);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, node.emergency_counter);
    for (int i = 0; i < NUM_EMERGENCY_BUFFER; i++) {
        TEST_ASSERT_EQUAL_INT(0, node.emergency_buffer[i]);
    }
}

// Guard test: invalid bit index
void test_RaiseEmergency_OutOfRangeBit(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);
    int invalid_bit = NUM_EMERGENCY_BUFFER * 8 + 1;
    int ret = EmergencyNode_raise(&node, invalid_bit);
    TEST_ASSERT_TRUE(ret != 0);
}

// Dependency enforcement test
void test_DependentEmergencySolve(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);

    printf("After init: Buffer[0]=%d, Counter=%d\n", node.emergency_buffer[0], node.emergency_counter);

    // Raise emergency 2 and 4
    EmergencyNode_raise(&node, 2);
    printf("After raise bit 2: Buffer[0]=%d, Counter=%d\n", node.emergency_buffer[0], node.emergency_counter);

    EmergencyNode_raise(&node, 4);
    printf("After raise bit 4: Buffer[0]=%d, Counter=%d\n", node.emergency_buffer[0], node.emergency_counter);

    // Try to solve emergency 4 - should FAIL because 2 is still active
    int ret = EmergencyNode_solve(&node, 4);
    TEST_ASSERT_TRUE(ret != 0);  // Expect failure due to dependency
    printf("After failed solve 4: Buffer[0]=%d, Counter=%d\n", node.emergency_buffer[0], node.emergency_counter);

    // Solve emergency 2 first
    ret = EmergencyNode_solve(&node, 2);
    TEST_ASSERT_EQUAL_INT(0, ret);
    printf("After solve 2: Buffer[0]=%d, Counter=%d\n", node.emergency_buffer[0], node.emergency_counter);

    // Now emergency 4 can be solved successfully
    ret = EmergencyNode_solve(&node, 4);
    TEST_ASSERT_EQUAL_INT(0, ret);
    printf("After solve 4: Buffer[0]=%d, Counter=%d\n", node.emergency_buffer[0], node.emergency_counter);
}

void test_RaiseEmergency_Performance(void) {
    EmergencyNode_t node;                        // Create our emergency node
    EmergencyNode_init(&node);                   // Initialize it to clean state
    
    const int num_operations = 1000;            // We'll do 1000 operations
    double start_time, end_time, total_time;    // Variables to store time measurements
    
    printf("Testing raise performance with %d operations...\n", num_operations);
    
    start_time = get_time_microseconds();       // Record the start time
    
    // This loop will raise 1000 emergencies
    for (int i = 0; i < num_operations; i++) {
        uint8_t bit = i % (NUM_EMERGENCY_BUFFER * 8);  // Choose which emergency bit to raise
        EmergencyNode_raise(&node, bit);               // Raise the emergency
    }
    
    end_time = get_time_microseconds();         // Record the end time
    total_time = end_time - start_time;         // Calculate how long it took
    
    printf("Total time: %.2f microseconds\n", total_time);
    printf("Average time per operation: %.2f microseconds\n", total_time / num_operations);
    
    // Test passes if it completes (we're measuring, not checking correctness here)
    TEST_ASSERT_TRUE(1);
}

void test_SolveEmergency_Performance(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);
    
    const int num_operations = 1000;
    double start_time, end_time, total_time;
    
    // First, raise many emergencies to have something to solve
    printf("Setting up %d emergencies to solve...\n", num_operations);
    for (int i = 0; i < num_operations; i++) {
        uint8_t bit = i % (NUM_EMERGENCY_BUFFER * 8);
        EmergencyNode_raise(&node, bit);
    }
    
    printf("Testing solve performance with %d operations...\n", num_operations);
    
    start_time = get_time_microseconds();       // Start timing
    
    // Now solve all the emergencies
    for (int i = 0; i < num_operations; i++) {
        uint8_t bit = i % (NUM_EMERGENCY_BUFFER * 8);
        EmergencyNode_solve(&node, bit);        // Solve each emergency
    }
    
    end_time = get_time_microseconds();         // Stop timing
    total_time = end_time - start_time;
    
    printf("Total time: %.2f microseconds\n", total_time);
    printf("Average time per operation: %.2f microseconds\n", total_time / num_operations);
    
    TEST_ASSERT_TRUE(1);
}

void test_Emergency_StressTest(void) {
    EmergencyNode_t node;
    EmergencyNode_init(&node);
    
    const int stress_operations = 10000;       // Much more operations for stress
    double start_time, end_time, total_time;
    
    printf("Running stress test with %d operations...\n", stress_operations);
    
    start_time = get_time_microseconds();
    
    // Mix of raise and solve operations
    for (int i = 0; i < stress_operations; i++) {
        uint8_t bit = i % (NUM_EMERGENCY_BUFFER * 8);
        
        if (i % 2 == 0) {                      // If i is even number
            EmergencyNode_raise(&node, bit);    // Raise emergency
        } else {                               // If i is odd number  
            EmergencyNode_solve(&node, bit);    // Solve emergency
        }
    }
    
    end_time = get_time_microseconds();
    total_time = end_time - start_time;
    
    printf("Stress test completed in %.2f microseconds\n", total_time);
    printf("Average time per mixed operation: %.2f microseconds\n", total_time / stress_operations);
    
    TEST_ASSERT_TRUE(1);
}



int main(void) {
    UNITY_BEGIN();
    
    // emergencies tests and depenmdance test
    RUN_TEST(test_NodeInitialization);
    RUN_TEST(test_RaiseEmergency);
    RUN_TEST(test_SolveEmergency);
    RUN_TEST(test_EmergencyStateCheck);
    RUN_TEST(test_DestroyEmergencyNode);
    RUN_TEST(test_RaiseEmergency_OutOfRangeBit);
    RUN_TEST(test_DependentEmergencySolve);
    
    // time tests
    RUN_TEST(test_RaiseEmergency_Performance);
    RUN_TEST(test_SolveEmergency_Performance);
    RUN_TEST(test_Emergency_StressTest);
    
    return UNITY_END();
}
