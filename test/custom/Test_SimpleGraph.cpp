#include "collabdata/custom/SimpleGraph.h"

#include <gtest/gtest.h>

#include "collabdata/custom/OperationObserver.h"


namespace collab {


static int nbCatch = 0;
static unsigned int localUserID = 42;


// /////////////////////////////////////////////////////////////////////////////
// Mock classes
// /////////////////////////////////////////////////////////////////////////////

static int nbOpVertexAdd    = 0;
static int nbOpVertexRemove = 0;
static int nbOpEdgeAdd      = 0;
static int nbOpEdgeRemove   = 0;
static int nbOpAttrAdd      = 0;
static int nbOpAttrRemove   = 0;
static int nbOpAttrSet      = 0;

static void resetNbOpCounters() {
    nbOpVertexAdd           = 0;
    nbOpVertexRemove        = 0;
    nbOpEdgeAdd             = 0;
    nbOpEdgeRemove          = 0;
    nbOpAttrAdd             = 0;
    nbOpAttrRemove          = 0;
    nbOpAttrSet             = 0;
}


class HandlerMock : public SimpleGraph::OpHandler {
    public:
        void handleOperation(const SimpleGraph::VertexAddOperation& op) {
            nbOpVertexAdd++;
        }
        void handleOperation(const SimpleGraph::VertexRemoveOperation& op) {
            nbOpVertexRemove++;
        }
        void handleOperation(const SimpleGraph::EdgeAddOperation& op) {
            nbOpEdgeAdd++;
        }
        void handleOperation(const SimpleGraph::EdgeRemoveOperation& op) {
            nbOpEdgeRemove++;
        }
        void handleOperation(const SimpleGraph::AttributeAddOperation& op) {
            nbOpAttrAdd++;
        }
        void handleOperation(const SimpleGraph::AttributeRemoveOperation& op) {
            nbOpAttrRemove++;
        }
        void handleOperation(const SimpleGraph::AttributeSetOperation& op) {
            nbOpAttrSet++;
        }
};

class ObserverMock : public OperationObserver {
    private:
        HandlerMock _handler;
    public:
        void onOperation(const Operation& op) override {
            op.accept(_handler);
        }
};


// /////////////////////////////////////////////////////////////////////////////
// Tests
// /////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// empty()
// -----------------------------------------------------------------------------

TEST(SimpleGraph, emptyTest) {
    SimpleGraph data0 = SimpleGraph::build(localUserID);
    ASSERT_TRUE(data0.empty());

    data0.addVertex("v1");
    ASSERT_FALSE(data0.empty());

    data0.removeVertex("v1");
    ASSERT_TRUE(data0.empty());
}


// -----------------------------------------------------------------------------
// at()
// -----------------------------------------------------------------------------

TEST(SimpleGraph, atTest_ThrowException) {
    SimpleGraph data0 = SimpleGraph::build(localUserID);
    nbCatch = 0;

    try { data0.at("v1"); } catch(...) { nbCatch++; }
    try { data0.at("v2"); } catch(...) { nbCatch++; }
    try { data0.at("v3"); } catch(...) { nbCatch++; }
    try { data0.at("v4"); } catch(...) { nbCatch++; }

    EXPECT_EQ(nbCatch, 4);

    nbCatch = 0;
    data0.addVertex("v1");
    data0.addVertex("v2");

    try { data0.at("v1"); } catch(...) { nbCatch++; }
    try { data0.at("v2"); } catch(...) { nbCatch++; }
    try { data0.at("v3"); } catch(...) { nbCatch++; }
    try { data0.at("v4"); } catch(...) { nbCatch++; }
    EXPECT_EQ(nbCatch, 2);
}

TEST(SimpleGraph, atTest_ReturnValue) {
    SimpleGraph data0 = SimpleGraph::build(localUserID);

    data0.addVertex("v1");
    data0.addVertex("v2");
    data0.addVertex("v3");
    data0.addEdge("v1", "v1");
    data0.addEdge("v1", "v2");
    data0.addEdge("v1", "v3");
    data0.addEdge("v2", "v1");

    data0.setAttribute("v1", "name", "Gnu");
    data0.setAttribute("v2", "name", "Panda");
    data0.setAttribute("v3", "name", "Corbeau");

    SimpleGraph::VertexDescriptor v1 = data0.at("v1");
    SimpleGraph::VertexDescriptor v2 = data0.at("v2");
    SimpleGraph::VertexDescriptor v3 = data0.at("v3");

    auto it_v1_attr = v1.attributes();
    it_v1_attr.moveNext();
    auto& current1 = it_v1_attr.current();
    EXPECT_EQ(current1.name(), "name");
    EXPECT_EQ(current1.value(), "Gnu");

    auto it_v2_attr = v2.attributes();
    it_v2_attr.moveNext();
    auto& current2 = it_v2_attr.current();
    EXPECT_EQ(current2.name(), "name");
    EXPECT_EQ(current2.value(), "Panda");

    auto it_v3_attr = v3.attributes();
    it_v3_attr.moveNext();
    auto& current3 = it_v3_attr.current();
    EXPECT_EQ(current3.name(), "name");
    EXPECT_EQ(current3.value(), "Corbeau");
}


// -----------------------------------------------------------------------------
// hasVertex()
// -----------------------------------------------------------------------------

TEST(SimpleGraph, hasVertexTest) {
    SimpleGraph data0 = SimpleGraph::build(localUserID);

    EXPECT_FALSE(data0.hasVertex("v1"));
    EXPECT_FALSE(data0.hasVertex("v2"));
    EXPECT_FALSE(data0.hasVertex("v3"));

    data0.addVertex("v1");
    data0.addVertex("v2");
    EXPECT_TRUE(data0.hasVertex("v1"));
    EXPECT_TRUE(data0.hasVertex("v2"));
    EXPECT_FALSE(data0.hasVertex("v3"));

    data0.removeVertex("v1");
    EXPECT_FALSE(data0.hasVertex("v1"));
    EXPECT_TRUE(data0.hasVertex("v2"));
    EXPECT_FALSE(data0.hasVertex("v3"));

    data0.removeVertex("v2");
    EXPECT_FALSE(data0.hasVertex("v1"));
    EXPECT_FALSE(data0.hasVertex("v2"));
    EXPECT_FALSE(data0.hasVertex("v3"));
}


// -----------------------------------------------------------------------------
// hasEdge()
// -----------------------------------------------------------------------------

TEST(SimpleGraph, hasEdgeTest) {
    SimpleGraph data0 = SimpleGraph::build(localUserID);

    EXPECT_FALSE(data0.hasEdge("v1", "v1"));
    EXPECT_FALSE(data0.hasEdge("v1", "v2"));
    EXPECT_FALSE(data0.hasEdge("v1", "v3"));

    data0.addEdge("v1", "v1");
    data0.addEdge("v1", "v2");
    EXPECT_TRUE(data0.hasEdge("v1", "v1"));
    EXPECT_TRUE(data0.hasEdge("v1", "v2"));
    EXPECT_FALSE(data0.hasEdge("v1", "v3"));

    data0.removeEdge("v1", "v1");
    EXPECT_FALSE(data0.hasEdge("v1", "v1"));
    EXPECT_TRUE(data0.hasEdge("v1", "v2"));
    EXPECT_FALSE(data0.hasEdge("v1", "v3"));

    data0.removeEdge("v1", "v2");
    EXPECT_FALSE(data0.hasEdge("v1", "v1"));
    EXPECT_FALSE(data0.hasEdge("v1", "v2"));
    EXPECT_FALSE(data0.hasEdge("v1", "v3"));
}


// -----------------------------------------------------------------------------
// OperationHandler
// -----------------------------------------------------------------------------

TEST(SimpleGraph, operationHandlersTest) {
    SimpleGraph data0 = SimpleGraph::build(localUserID);
    ObserverMock obs;
    data0.addOperationObserver(obs);

    resetNbOpCounters();

    data0.addVertex("v1");
    data0.addVertex("v1");
    data0.addVertex("v1");
    data0.addVertex("v1");
    ASSERT_EQ(nbOpVertexAdd, 1);

    data0.addVertex("v2");
    data0.addVertex("v2");
    data0.addVertex("v2");
    ASSERT_EQ(nbOpVertexAdd, 2);

    data0.addVertex("v3");
    data0.addVertex("v3");
    data0.addVertex("v3");
    ASSERT_EQ(nbOpVertexAdd, 3);

    data0.addVertex("v3");
    data0.addVertex("v2");
    data0.addVertex("v1");
    ASSERT_EQ(nbOpVertexAdd, 3);

    data0.removeEdge("v1", "v2");
    data0.removeEdge("v1", "v2");
    data0.removeEdge("v1", "v2");
    ASSERT_EQ(nbOpEdgeRemove, 0);

    data0.addEdge("v1", "v2");
    data0.addEdge("v1", "v2");
    data0.addEdge("v1", "v2");
    ASSERT_EQ(nbOpEdgeAdd, 1);
}


} // End namespace


