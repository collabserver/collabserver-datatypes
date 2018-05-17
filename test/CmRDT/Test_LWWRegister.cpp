#include "CmRDT/LWWRegister.h"
#include <gtest/gtest.h>

namespace CRDT {
namespace CmRDT {


// -----------------------------------------------------------------------------
// Class method tests
// -----------------------------------------------------------------------------

TEST(LWWRegister, queryTest) {
    LWWRegister<int, int> data0;

    data0.update(42, 1);
    ASSERT_TRUE(data0.query() == 42);

    data0.update(100, 2);
    ASSERT_TRUE(data0.query() == 100);

    data0.update(300, 3);
    ASSERT_TRUE(data0.query() == 300);
}

TEST(LWWRegister, updateTest) {
    LWWRegister<int, int> data0;

    // Simulate different add at timestamps
    data0.update(666, 6);
    ASSERT_EQ(data0.query(), 666);

    data0.update(555, 5);
    ASSERT_EQ(data0.query(), 666);

    data0.update(888, 8);
    ASSERT_EQ(data0.query(), 888);

    data0.update(777, 7);
    ASSERT_EQ(data0.query(), 888);

    data0.update(999, 9);
    ASSERT_EQ(data0.query(), 999);

    data0.update(444, 4);
    ASSERT_EQ(data0.query(), 999);

    data0.update(222, 2);
    ASSERT_EQ(data0.query(), 999);

    data0.update(111, 1);
    ASSERT_EQ(data0.query(), 999);

    data0.update(333, 3);
    ASSERT_EQ(data0.query(), 999);
}


// -----------------------------------------------------------------------------
// Operator Tests
// -----------------------------------------------------------------------------

TEST(LWWRegister, operatorEQTest) {
    LWWRegister<int, int> data0;
    LWWRegister<int, int> data1;

    // Exact EQ
    data0.update(42, 1);
    data1.update(42, 1);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);

    // Register with different timestamp are equal anyway
    data0.update(77, 7);
    data1.update(77, 9);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);
}


// -----------------------------------------------------------------------------
// Use case Tests
// -----------------------------------------------------------------------------

TEST(LWWRegister, useCaseNormalTest) {
    LWWRegister<int, int> data0;
    LWWRegister<int, int> data1;


    // At replicate 0
    data0.update(300, 3);
    ASSERT_EQ(data0.query(), 300);

    data0.update(100, 1);
    ASSERT_EQ(data0.query(), 300);

    data0.update(500, 5);
    ASSERT_EQ(data0.query(), 500);

    data0.update(700, 7);
    ASSERT_EQ(data0.query(), 700);

    data0.update(200, 2);
    ASSERT_EQ(data0.query(), 700);


    // At replicate 1
    data1.update(320, 3);
    ASSERT_EQ(data1.query(), 320);

    data1.update(100, 1);
    ASSERT_EQ(data1.query(), 320);

    data1.update(500, 5);
    ASSERT_EQ(data1.query(), 500);

    data1.update(700, 7);
    ASSERT_EQ(data1.query(), 700);

    ASSERT_EQ(data0.query(), data1.query());
}


}} // End namespaces


