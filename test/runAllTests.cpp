/*
 * TEST NAMING RULE (For all test files):
 * The first word of test name is generally the tested method
 * ex: addDuplicateCallsTest to test function 'add'
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
