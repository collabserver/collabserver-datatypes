#include "CmRDT/LWWMap.h"
#include <gtest/gtest.h>

namespace CRDT {
namespace CmRDT {


// -----------------------------------------------------------------------------
// CRDT methods tests
// -----------------------------------------------------------------------------

TEST(LWWMap, queryTest) {
    LWWMap<int, int, int> data0;

    // Query before even exists
    auto res = data0.query(1);
    EXPECT_TRUE(res == data0.lend());

    // Add element
    data0.add(1, 10);
    res = data0.query(1);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 1);
    EXPECT_FALSE(res->second._isRemoved);

    // Remove element. Query still works
    data0.remove(1, 20);
    res = data0.query(1);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 1);
    EXPECT_TRUE(res->second._isRemoved);

    // Query another element before even added
    res = data0.query(2);
    EXPECT_TRUE(res == data0.lend());

    // Add this element
    data0.add(2, 30);
    res = data0.query(2);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 2);
    EXPECT_FALSE(res->second._isRemoved);
}

TEST(LWWMap, addTest) {
    LWWMap<int, int, int> data0;

    // Replicate 0
    data0.add(0, 10);
    data0.add(1, 11);
    data0.add(2, 12);
    data0.add(3, 13);
    for(int k = 0; k < 4; ++k) {
        auto res = data0.query(k);
        EXPECT_TRUE(res != data0.lend());
        EXPECT_EQ(res->first, k);
        EXPECT_FALSE(res->second._isRemoved);
    }
}

TEST(LWWMap, removeTest) {
    LWWMap<int, int, int> data0;

    // Remove before even addded works
    data0.remove(0, 10);
    auto res = data0.query(0);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 0);
    EXPECT_TRUE(res->second._isRemoved);

    // Replicate 0
    data0.add(0, 20);
    data0.add(1, 21);
    data0.add(2, 22);
    data0.add(3, 23);
    data0.remove(0, 30);
    data0.remove(1, 31);
    data0.remove(2, 32);
    data0.remove(3, 33);
    for(int k = 0; k < 4; ++k) {
        res = data0.query(k);
        EXPECT_TRUE(res != data0.lend());
        EXPECT_EQ(res->first, k);
        EXPECT_TRUE(res->second._isRemoved);
    }
}

TEST(LWWMap, addRemoveTest) {
    LWWMap<int, int, int> data0;

    // Receive the remove (Made last, but received first for whatever reasons)
    data0.remove(42, 1000);
    auto res = data0.query(42);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->first, 42);

    // Receive add that was actually before (Stay removed)
    data0.add(42, 10);
    data0.add(42, 11);
    data0.add(42, 12);
    res = data0.query(42);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->first, 42);

    // Re-add element later (Is now added)
    data0.add(42, 1001);
    res = data0.query(42);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_FALSE(res->second._isRemoved);
    EXPECT_EQ(res->first, 42);

    // Receive remove that was older (Stay added)
    data0.remove(42, 20);
    res = data0.query(42);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_FALSE(res->second._isRemoved);
    EXPECT_EQ(res->first, 42);
}


// -----------------------------------------------------------------------------
// Operator tests
// -----------------------------------------------------------------------------

TEST(LWWMap, operatorEQTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // EQ. Timestamp doesn't count
    data0.add("attr1", 10);
    data1.add("attr1", 11);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);

    // Removed element are not used for equality
    data0.remove("attr2", 20);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);

    // Also remove the other, but doesn't change. Still equals
    data1.remove("attr2", 30);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);

    // Some more add (Almost same but not same)
    data0.add("attr3", 40);
    data0.add("attr4", 41);
    data0.add("attr5", 42);
    data1.add("attr3", 43);
    data1.add("attr4", 44);
    ASSERT_FALSE(data0 == data1);
    ASSERT_TRUE(data0 != data1);

    // Back to same
    data1.add("attr5", 50);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);
}


// -----------------------------------------------------------------------------
// Use Case Tests
// -----------------------------------------------------------------------------

TEST(LWWMap, useCaseAddRemoveTest) {
    // TODO test to do
}


}} // End namespaces


