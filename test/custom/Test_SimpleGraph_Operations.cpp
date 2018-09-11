#include "collabdata/custom/SimpleGraph.h"

#include <gtest/gtest.h>

#include "collabdata/custom/Timestamp.h"

namespace collab {


TEST(SimpleGraph, opEdgeAddOperation_serializeUnserialize) {
    Timestamp::setEffectiveID(42);
    Timestamp time = Timestamp::now();
    SimpleGraph::EdgeAddOperation op = {"vertexNumber1", "vertexNumber2", time};
    std::stringstream buffer;

    Timestamp oldTime = op.timestamp();

    EXPECT_TRUE(op.serialize(buffer));
    EXPECT_TRUE(op.unserialize(buffer));
    ASSERT_EQ(op.fromID(), "vertexNumber1");
    ASSERT_EQ(op.toID(), "vertexNumber2");
    ASSERT_EQ(op.timestamp().getTime(), oldTime.getTime());
}



} // End namespace


