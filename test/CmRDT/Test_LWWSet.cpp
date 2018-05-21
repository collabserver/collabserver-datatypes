#include "CmRDT/LWWSet.h"
#include <gtest/gtest.h>

namespace CRDT {
namespace CmRDT {


// -----------------------------------------------------------------------------
// CRDT method tests
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
// Iterator Tests (Normal iterator)
// -----------------------------------------------------------------------------

TEST(LWWSet, iteratorAddRemoveTest) {
    LWWSet<int, int> data0;

    // Add some elements and test iteration
    data0.add(0, 10);
    data0.add(1, 11);
    data0.add(2, 12);
    data0.add(3, 13);
    int k = 0;
    for(auto it = data0.begin(); it != data0.end(); ++it) {
        // Dev note: I'm not sure order is predictable. I use nb of iteration instead.
        ++k;
    }
    EXPECT_EQ(k, 4);

    // Remove elements, then iterator should not use them
    data0.remove(0, 20);
    data0.remove(1, 21);
    k = 0;
    for(auto& elt : data0) {
        ++k;
    }
    EXPECT_EQ(k, 2);

    // Add again some more and test
    data0.add(4, 30);
    data0.add(5, 31);
    data0.add(6, 32);
    data0.add(7, 33);
    k = 0;
    for(auto& elt : data0) {
        ++k;
    }
    EXPECT_EQ(k, 6);
}

TEST(LWWSet, iteratorEmptySetTest) {
    LWWSet<int, int> data0;

    // Iterate empty set should be ok (No elt)
    for(auto& elt : data0) {
        EXPECT_TRUE(false) << "Iterator should be empty but found " << elt;
    }

    // Add / remove some elements and iterate
    data0.add(0, 10);
    data0.add(1, 11);
    data0.add(2, 12);
    data0.add(3, 13);
    data0.add(4, 14);
    data0.remove(0, 20);
    data0.remove(1, 21);
    data0.remove(2, 22);
    data0.remove(3, 23);
    data0.remove(4, 24);
    for(auto& elt : data0) {
        // data0 should be empty from iterator point of view
        ASSERT_TRUE(false) << "Iterator should be empty but found " << elt;
    }

    // Add more and remove
    data0.add(5, 30);
    data0.remove(5, 31);
    for(auto& elt : data0) {
        // Should be Still empty
        ASSERT_TRUE(false) << "Iterator should be empty but found " << elt;
    }

    // Add / remove again
    data0.add(6, 40);
    data0.add(7, 41);
    data0.add(8, 42);
    data0.add(9, 43);
    data0.remove(6, 44);
    data0.remove(7, 45);
    data0.remove(8, 46);
    data0.remove(9, 47);
    for(auto& elt : data0) {
        // Should be Still empty
        ASSERT_TRUE(false) << "Iterator should be empty but found " << elt;
    }
}


// -----------------------------------------------------------------------------
// Operator Tests
// -----------------------------------------------------------------------------

TEST(LWWSet, operatorEQTest) {
    LWWSet<std::string, int> data0;
    LWWSet<std::string, int> data1;

    // Simple add, both are same (Timestamp doesn't count)
    data0.add("v1", 1);
    data1.add("v1", 2);
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);

    // Removed elt not used in equality
    data0.remove("v2", 3);
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);

    // Back to same
    data1.remove("v2", 3);
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);

    // Some more add
    data0.add("v3", 4);
    data0.add("v4", 5);
    data0.add("v5", 6);
    data1.add("v3", 7);
    data1.add("v4", 8);
    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);
    data1.add("v5", 9);
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);
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


