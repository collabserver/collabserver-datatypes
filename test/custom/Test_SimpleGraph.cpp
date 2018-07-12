#include "collabdata/custom/SimpleGraph.h"

#include <gtest/gtest.h>

#include "collabdata/custom/OperationObserver.h"


namespace collab {


// /////////////////////////////////////////////////////////////////////////////
// *****************************************************************************
// Mock classes
// *****************************************************************************
// /////////////////////////////////////////////////////////////////////////////


static int nbOpVertexAdd = 0;
static int nbOpVertexRemove = 0;
static int nbOpEdgeAdd = 0;
static int nbOpEdgeRemove = 0;
static int nbOpAttrAdd = 0;
static int nbOpAttrRemove = 0;
static int nbOpAttrSet = 0;

static void resetNbOpCounters() {
    nbOpVertexAdd = 0;
    nbOpVertexRemove = 0;
    nbOpEdgeAdd = 0;
    nbOpEdgeRemove = 0;
    nbOpAttrAdd = 0;
    nbOpAttrRemove = 0;
    nbOpAttrSet = 0;
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
        void onOperation(const Operation& op) {
            op.handle(_handler);
        }
};


// /////////////////////////////////////////////////////////////////////////////
// *****************************************************************************
// Tests
// *****************************************************************************
// /////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// constructor()
// -----------------------------------------------------------------------------

TEST(SimpleGraph, constructorTest) {
    SimpleGraph v;
    // TODO
}

TEST(SimpleGraph, operationHandlersTest) {
    SimpleGraph data0;
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


