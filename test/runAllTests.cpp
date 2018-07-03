/*
 * TEST NAMING RULE (For all test files):
 *
 * MethodName_stateUnderTest_expectedBehavior
 *
 * MethodName is the method name in cameCase + Test
 * Example: addVertexTest_concurrentRemoveEdge
 *
 * General cases to check for any CRDT data:
 *  - Normal example
 *  - Indenpotent
 *  - Commutativity (Duplicat calls with diff timestamps)
 *  - Return type
 *  - Called at usually invalid place (Remove before add).
 */

#include <gtest/gtest.h>


/*
 * Start your engiiiiines!!! Yeah!
 * Let's hope they will all pass! (And they will! Yes yes!)
 */
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
