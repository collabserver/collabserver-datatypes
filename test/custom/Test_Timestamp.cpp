#include "collabdata/custom/Timestamp.h"

#include <gtest/gtest.h>

namespace collab {


// -----------------------------------------------------------------------------
// operators=()
// -----------------------------------------------------------------------------

TEST(Timestamp, operatorAssign) {
    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t1 < t2);
    ASSERT_TRUE(t2 > t1);
}


// -----------------------------------------------------------------------------
// operators==()
// -----------------------------------------------------------------------------

TEST(Timestamp, operatorEQ) {
    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t1 == t1);
    ASSERT_TRUE(t2 == t2);

    ASSERT_FALSE(t1 == t2);
    ASSERT_FALSE(t2 == t1);
}

TEST(Timestamp, operatorEQ_WithDiffEffectiveID) {
    Timestamp::effectiveID(42);
    Timestamp t1 = {0};
    Timestamp::effectiveID(64);
    Timestamp t2 = {0};

    ASSERT_TRUE(t1 == t1);
    ASSERT_TRUE(t2 == t2);

    ASSERT_FALSE(t1 == t2);
    ASSERT_FALSE(t2 == t1);
}


// -----------------------------------------------------------------------------
// operators<()
// -----------------------------------------------------------------------------

TEST(Timestamp, operatorInf) {
    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t1 < t2);
    ASSERT_FALSE(t2 < t1);
}

TEST(Timestamp, operatorInf_WithDiffEffectiveID) {
    Timestamp::effectiveID(10);
    Timestamp t1 = {0};
    Timestamp::effectiveID(20);
    Timestamp t2 = {0};

    ASSERT_TRUE(t1 < t2);
    ASSERT_FALSE(t2 < t1);
}


// -----------------------------------------------------------------------------
// operators>()
// -----------------------------------------------------------------------------

TEST(Timestamp, operatorSup) {
    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t2 > t1);
    ASSERT_FALSE(t1 > t2);
}

TEST(Timestamp, operatorSup_WithDiffEffectiveID) {
    Timestamp::effectiveID(10);
    Timestamp t1 = {0};
    Timestamp::effectiveID(20);
    Timestamp t2 = {0};

    ASSERT_TRUE(t2 > t1);
    ASSERT_FALSE(t1 > t2);
}


} // End namespace


