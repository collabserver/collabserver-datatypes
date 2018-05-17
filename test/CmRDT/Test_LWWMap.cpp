#include "CmRDT/LWWMap.h"
#include <gtest/gtest.h>

namespace CRDT {
namespace CmRDT {


// -----------------------------------------------------------------------------
// CRDT methods tests
// -----------------------------------------------------------------------------

TEST(LWWMap, queryTest) {
    EXPECT_TRUE(false) << "TODO Test";
}

TEST(LWWMap, addTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // Replicate 0
    data0.add("attr1", 1);
    data0.add("attr1", 2);
    data0.add("attr1", 3);

    // Replicate 1
    data1.add("attr1", 2);
    data1.add("attr1", 3);
    data1.add("attr1", 1);

    EXPECT_TRUE(false) << "TODO query method";
}

TEST(LWWMap, removeTest) {
    EXPECT_TRUE(false) << "TODO Test";
}


// -----------------------------------------------------------------------------
// Operator tests
// -----------------------------------------------------------------------------

TEST(LWWMap, operatorEQTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // Exact EQ
    data0.add("attr1", 1);
    data1.add("attr1", 1);
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);
}


// -----------------------------------------------------------------------------
// Use Case Tests
// -----------------------------------------------------------------------------

TEST(LWWMap, useCaseAddRemoveTest) {
    EXPECT_TRUE(false) << "TODO Test";
}


}} // End namespaces


