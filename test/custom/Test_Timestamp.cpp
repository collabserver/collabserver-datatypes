#include "collabdata/custom/Timestamp.h"

#include <gtest/gtest.h>

namespace collab {


TEST(Timestamp, operatorAssignTest) {
    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();

    ASSERT_TRUE(t1 < t2);
    ASSERT_TRUE(t2 > t1);
}

TEST(Timestamp, operatorEQ) {
    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();
    ASSERT_TRUE(t1 == t1);
    ASSERT_TRUE(t2 == t2);
}

TEST(Timestamp, operatorInf) {
    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();
    ASSERT_TRUE(t1 < t2);
}

TEST(Timestamp, operatorSupEQ) {
    Timestamp t1 = {0};
    Timestamp t2 = Timestamp::now();
    ASSERT_TRUE(t2 > t1);
}


} // End namespace


