#include "collabdata/custom/Timestamp.h"

#include <gtest/gtest.h>

namespace collab {

static unsigned int localUserID = 42; // See SimpleGraph constructor and Timestamp


// -----------------------------------------------------------------------------
// operators=()
// -----------------------------------------------------------------------------

TEST(Timestamp, operatorAssign) {
    Timestamp::setEffectiveID(localUserID);

    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t1 < t2);
    ASSERT_TRUE(t2 > t1);
}


// -----------------------------------------------------------------------------
// operators==()
// -----------------------------------------------------------------------------

TEST(Timestamp, operatorEQ) {
    Timestamp::setEffectiveID(localUserID);

    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t1 == t1);
    ASSERT_TRUE(t2 == t2);

    ASSERT_FALSE(t1 == t2);
    ASSERT_FALSE(t2 == t1);
}

TEST(Timestamp, operatorEQ_WithDiffEffectiveID) {
    Timestamp::setEffectiveID(localUserID);
    Timestamp t1 = {0};
    Timestamp::setEffectiveID(64); // ex: User2
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
    Timestamp::setEffectiveID(localUserID);

    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t1 < t2);
    ASSERT_FALSE(t2 < t1);
}

TEST(Timestamp, operatorInf_WithDiffEffectiveID) {
    Timestamp::setEffectiveID(10);
    Timestamp t1 = {0};
    Timestamp::setEffectiveID(20);
    Timestamp t2 = {0};

    ASSERT_TRUE(t1 < t2);
    ASSERT_FALSE(t2 < t1);
}


// -----------------------------------------------------------------------------
// operators>()
// -----------------------------------------------------------------------------

TEST(Timestamp, operatorSup) {
    Timestamp::setEffectiveID(localUserID);

    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t2 > t1);
    ASSERT_FALSE(t1 > t2);
}

TEST(Timestamp, operatorSup_WithDiffEffectiveID) {
    Timestamp::setEffectiveID(10);
    Timestamp t1 = {0};
    Timestamp::setEffectiveID(20);
    Timestamp t2 = {0};

    ASSERT_TRUE(t2 > t1);
    ASSERT_FALSE(t1 > t2);
}


} // End namespace


