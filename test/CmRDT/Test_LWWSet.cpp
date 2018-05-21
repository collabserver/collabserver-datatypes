#include "CmRDT/LWWSet.h"
#include <gtest/gtest.h>

namespace CRDT {
namespace CmRDT {


// -----------------------------------------------------------------------------
// CRDT method tests
// -----------------------------------------------------------------------------

TEST(LWWSet, queryTest) {
    LWWSet<std::string, int> data0;

    // Query before exists
    auto res = data0.query("e1");
    EXPECT_TRUE(res == data0.lend());

    // Add element and query
    data0.add("e1", 10);
    res = data0.query("e1");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "e1");
    EXPECT_FALSE(res->second._isRemoved);

    // Remove this element and query
    data0.remove("e1", 20);
    res = data0.query("e1");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "e1");
    EXPECT_TRUE(res->second._isRemoved);

    // Query invalid data
    res = data0.query("xxx");
    EXPECT_TRUE(res == data0.lend());
}

TEST(LWWSet, addTest) {
    LWWSet<std::string, int> data0;

    // Add normal
    data0.add("e1", 1);
    data0.add("e2", 2);
    data0.add("e3", 3);
    data0.add("e4", 4);

    auto res = data0.query("e1");
    EXPECT_TRUE(res != data0.lend() && res->first == "e1" && res->second._isRemoved == false);

    res = data0.query("e2");
    EXPECT_TRUE(res != data0.lend() && res->first == "e2" && res->second._isRemoved == false);

    res = data0.query("e3");
    EXPECT_TRUE(res != data0.lend() && res->first == "e3" && res->second._isRemoved == false);

    res = data0.query("e4");
    EXPECT_TRUE(res != data0.lend() && res->first == "e4" && res->second._isRemoved == false);
}

TEST(LWWSet, removeTest) {
    LWWSet<std::string, int> data0;

    // Remove before add is valid
    data0.remove("e1", 10);
    auto res = data0.query("e1");
    EXPECT_TRUE(res != data0.lend() && res->first == "e1" && res->second._isRemoved == true);

    // Add then remove
    data0.add("e1", 21);
    data0.add("e2", 22);
    data0.add("e3", 23);
    data0.add("e4", 24);
    data0.remove("e1", 31);
    data0.remove("e2", 32);
    data0.remove("e3", 33);
    data0.remove("e4", 34);
    auto res1 = data0.query("e1");
    auto res2 = data0.query("e2");
    auto res3 = data0.query("e3");
    auto res4 = data0.query("e4");
    EXPECT_TRUE(res1 != data0.lend() && res1->first == "e1" && res1->second._isRemoved == true);
    EXPECT_TRUE(res2 != data0.lend() && res2->first == "e2" && res2->second._isRemoved == true);
    EXPECT_TRUE(res3 != data0.lend() && res3->first == "e3" && res3->second._isRemoved == true);
    EXPECT_TRUE(res4 != data0.lend() && res4->first == "e4" && res4->second._isRemoved == true);
}

TEST(LWWSet, addRemoveTest) {
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

TEST(LWWSet, iteratorReferenceTest) {
    LWWSet<int, int> data0;

    // Add all
    data0.add(1, 1);
    auto it = data0.begin();
    EXPECT_EQ(*it, 1);
}


// -----------------------------------------------------------------------------
// Iterator Tests (Load iterator)
// -----------------------------------------------------------------------------

TEST(LWWSet, loadIteratorAddRemoveTest) {
    LWWSet<int, int> data0;

    // Add some elements and test iteration
    data0.add(0, 10);
    data0.add(1, 11);
    data0.add(2, 12);
    data0.add(3, 13);
    int k = 0;
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        // Dev note: I'm not sure order is predictable. I use nb of iteration instead.
        ++k;
    }
    EXPECT_EQ(k, 4);

    // Remove elements, should not impact load iterator
    data0.remove(0, 20);
    data0.remove(1, 21);
    k = 0;
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        ++k;
    }
    EXPECT_EQ(k, 4);

    // Add / remove some more
    data0.add(4, 30);
    data0.add(5, 31);
    data0.add(6, 32);
    data0.add(7, 33);
    data0.remove(4, 34);
    data0.remove(5, 35);
    k = 0;
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        ++k;
    }
    EXPECT_EQ(k, 8);
}

TEST(LWWSet, loadIteratorEmptyTest) {
    LWWSet<int, int> data0;

    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        ASSERT_TRUE(false) << "Load Iterator should be empty";;
    }
}

TEST(LWWSet, loadIteratorRemovedTest) {
    LWWSet<int, int> data0;

    // Fill set with removed elt (Yes, we don't even need add before).
    data0.remove(1, 10);
    data0.remove(2, 11);
    data0.remove(3, 12);
    data0.remove(4, 13);
    data0.remove(5, 14);
    data0.remove(5, 15);
    int k = 0;
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        ++k;
    }
    ASSERT_EQ(k, 5);
}

TEST(LWWSet, loadIteratorReferenceTest) {
    LWWSet<int, int> data0;

    // Simple test add then remove
    data0.add(1, 10);
    auto it = data0.lbegin();
    EXPECT_FALSE(it->second._isRemoved);
    it = data0.lbegin();
    data0.remove(1, 20);
    EXPECT_TRUE(it->second._isRemoved);

    // Add all
    data0.add(1, 21);
    data0.add(2, 22);
    data0.add(3, 23);
    data0.add(4, 24);
    data0.add(5, 25);
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        EXPECT_FALSE(it->second._isRemoved);
    }

    // Remove all
    data0.remove(1, 31);
    data0.remove(2, 32);
    data0.remove(3, 33);
    data0.remove(4, 34);
    data0.remove(5, 35);
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        EXPECT_TRUE(it->second._isRemoved);
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


}} // End namespaces


