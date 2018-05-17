#include "CmRDT/LWWSet.h"
#include <gtest/gtest.h>

namespace CRDT {
namespace CmRDT {


// -----------------------------------------------------------------------------
// Class method tests
// -----------------------------------------------------------------------------

TEST(LWWSet, queryTest) {
    ASSERT_TRUE(false) << "TODO Test";
}

TEST(LWWSet, addTest) {
    LWWSet<std::string, int> data0;

    // Add normal
    data0.add("e1", 1);
    data0.add("e2", 2);
    data0.add("e3", 3);
    data0.add("e4", 4);

    // TODO query
    EXPECT_TRUE(false) << "TODO Query not implemented yet";
}

TEST(LWWSet, removeTest) {
    EXPECT_TRUE(false) << "TODO test";
    // TODO query
    EXPECT_TRUE(false) << "TODO Query not implemented yet";
}


// -----------------------------------------------------------------------------
// Operator Tests
// -----------------------------------------------------------------------------

TEST(LWWSet, operatorEQTest) {
    LWWSet<std::string, int> data0;
    LWWSet<std::string, int> data1;

    // Simple add
    data0.add("v1", 1);
    data1.add("v1", 1);
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);

    // Removed elt doesn't count
    data0.remove("v2", 2);
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);

    // Back to same
    data1.remove("v2", 2);
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);

    // Internal metadata such as Timestamp doesn't count
    LWWSet<std::string, int> data2;
    LWWSet<std::string, int> data3;
    data2.add("v1", 1);
    data3.add("v1", 2);
    EXPECT_TRUE(data2 == data3);
    EXPECT_FALSE(data2 != data3);
}


// -----------------------------------------------------------------------------
// Use case Tests
// -----------------------------------------------------------------------------

TEST(LWWSet, useCaseAddRemoveTest) {
    LWWSet<std::string, int> data0;
    LWWSet<std::string, int> data1;

    // User1 flow (Normal order)
    data0.add("v1", 1);
    data0.add("v2", 3);
    data0.remove("v1", 4);

    // User1 flow (Remove before add)
    data1.remove("v1", 4);
    data1.add("v2", 3);
    data1.add("v1", 1);

    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);
}


}} // End namespaces


