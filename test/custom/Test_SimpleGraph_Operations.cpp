#include "collabdata/custom/SimpleGraph.h"

#include <gtest/gtest.h>

#include "collabdata/custom/Timestamp.h"

namespace collab {


static unsigned int localUserID = 42; // See SimpleGraph constructor and Timestamp


// -----------------------------------------------------------------------------
// VertexAddOperation
// -----------------------------------------------------------------------------

TEST(SimpleGraph, vertexAddOperation_serializeUnserialize) {
    const char* vertexID = "Superlong vertex name of the year. Panda is hungry!";
    Timestamp::setEffectiveID(localUserID);

    Timestamp time = Timestamp::now();
    SimpleGraph::VertexAddOperation op = {vertexID, time};
    std::stringstream buffer;

    Timestamp oldTime = op.getTimestamp();

    EXPECT_TRUE(op.serialize(buffer));
    EXPECT_TRUE(op.unserialize(buffer));

    ASSERT_EQ(op.getVertexID(), vertexID);
    ASSERT_EQ(op.getTimestamp().getTime(), oldTime.getTime());
}


// -----------------------------------------------------------------------------
// VertexRemoveOperation
// -----------------------------------------------------------------------------

TEST(SimpleGraph, vertexRemoveOperation_serializeUnserialize) {
    const char* vertexID = "Superlong vertex name of the year. Panda is hungry!";
    Timestamp::setEffectiveID(localUserID);

    Timestamp time = Timestamp::now();
    SimpleGraph::VertexRemoveOperation op = {vertexID, time};
    std::stringstream buffer;

    Timestamp oldTime = op.getTimestamp();

    EXPECT_TRUE(op.serialize(buffer));
    EXPECT_TRUE(op.unserialize(buffer));

    ASSERT_EQ(op.getVertexID(), vertexID);
    ASSERT_EQ(op.getTimestamp().getTime(), oldTime.getTime());
}


// -----------------------------------------------------------------------------
// EdgeAddOperation
// -----------------------------------------------------------------------------

TEST(SimpleGraph, edgeAddOperation_serializeUnserialize) {
    const char* vertex1 = "The Vertex number one (fromID)";
    const char* vertex2 = "The Vertex number two (toID)";
    Timestamp::setEffectiveID(localUserID);

    Timestamp time = Timestamp::now();
    SimpleGraph::EdgeAddOperation op = {vertex1, vertex2, time};
    std::stringstream buffer;

    Timestamp oldTime = op.getTimestamp();

    EXPECT_TRUE(op.serialize(buffer));
    EXPECT_TRUE(op.unserialize(buffer));

    ASSERT_EQ(op.getFromID(), vertex1);
    ASSERT_EQ(op.getToID(), vertex2);
    ASSERT_EQ(op.getTimestamp().getTime(), oldTime.getTime());
}


// -----------------------------------------------------------------------------
// EdgeRemoveOperation
// -----------------------------------------------------------------------------

TEST(SimpleGraph, edgeRemoveOperation_serializeUnserialize) {
    const char* vertex1 = "The Vertex number one (fromID)";
    const char* vertex2 = "The Vertex number two (toID)";
    Timestamp::setEffectiveID(localUserID);

    Timestamp time = Timestamp::now();
    SimpleGraph::EdgeRemoveOperation op = {vertex1, vertex2, time};
    std::stringstream buffer;

    Timestamp oldTime = op.getTimestamp();

    EXPECT_TRUE(op.serialize(buffer));
    EXPECT_TRUE(op.unserialize(buffer));

    ASSERT_EQ(op.getFromID(), vertex1);
    ASSERT_EQ(op.getToID(), vertex2);
    ASSERT_EQ(op.getTimestamp().getTime(), oldTime.getTime());
}


// -----------------------------------------------------------------------------
// AttributeAddOperation
// -----------------------------------------------------------------------------

TEST(SimpleGraph, attributeAddOperation_serializeUnserialize) {
    const char* vertexID = "A vertex name here. This is a long one (For test)";
    const char* attrName = "Name of the attribute. This is a long one";
    const char* attrValue = "The attribute value. Blablabla Blablabla food!";
    Timestamp::setEffectiveID(localUserID);

    Timestamp time = Timestamp::now();
    SimpleGraph::AttributeAddOperation op = {vertexID, time, attrName, attrValue};
    std::stringstream buffer;

    Timestamp oldTime = op.getTimestamp();

    EXPECT_TRUE(op.serialize(buffer));
    EXPECT_TRUE(op.unserialize(buffer));

    ASSERT_EQ(op.getVertexID(), vertexID);
    ASSERT_EQ(op.getTimestamp().getTime(), oldTime.getTime());
    ASSERT_EQ(op.getAttributeName(), attrName);
    ASSERT_EQ(op.getAttributeValue(), attrValue);
}


// -----------------------------------------------------------------------------
// AttributeRemoveOperation
// -----------------------------------------------------------------------------

TEST(SimpleGraph, attributeRemoveOperation_serializeUnserialize) {
    const char* vertexID = "A vertex name here. This is a long one (For test)";
    const char* attrName = "Name of the attribute. This is a long one";
    Timestamp::setEffectiveID(localUserID);

    Timestamp time = Timestamp::now();
    SimpleGraph::AttributeRemoveOperation op = {vertexID, time, attrName};
    std::stringstream buffer;

    Timestamp oldTime = op.getTimestamp();

    EXPECT_TRUE(op.serialize(buffer));
    EXPECT_TRUE(op.unserialize(buffer));

    ASSERT_EQ(op.getVertexID(), vertexID);
    ASSERT_EQ(op.getTimestamp().getTime(), oldTime.getTime());
    ASSERT_EQ(op.getAttributeName(), attrName);
}


// -----------------------------------------------------------------------------
// AttributeSetOperation
// -----------------------------------------------------------------------------

TEST(SimpleGraph, attributeSetOperation_serializeUnserialize) {
    const char* vertexID = "A vertex name here. This is a long one (For test)";
    const char* attrName = "Name of the attribute. This is a long one";
    const char* attrValue = "The attribute value. Blablabla Blablabla food!";
    Timestamp::setEffectiveID(localUserID);

    Timestamp time = Timestamp::now();
    SimpleGraph::AttributeSetOperation op = {vertexID, time, attrName, attrValue};
    std::stringstream buffer;

    Timestamp oldTime = op.getTimestamp();

    EXPECT_TRUE(op.serialize(buffer));
    EXPECT_TRUE(op.unserialize(buffer));

    ASSERT_EQ(op.getVertexID(), vertexID);
    ASSERT_EQ(op.getTimestamp().getTime(), oldTime.getTime());
    ASSERT_EQ(op.getAttributeName(), attrName);
    ASSERT_EQ(op.getAttributeValue(), attrValue);
}


} // End namespace


