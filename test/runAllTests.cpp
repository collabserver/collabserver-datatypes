/*
 * Dev note for all test files:
 *
 * The first word of test name is generally the tested methode.
 * ex: addDuplicateCallsTest -> test function 'add'
 */

#include <gtest/gtest.h>


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
