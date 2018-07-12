#include "collabdata/custom/SimpleGraph.h"

#include <gtest/gtest.h>

#include "collabdata/custom/OperationObserver.h"


namespace collab {


// /////////////////////////////////////////////////////////////////////////////
// *****************************************************************************
// Mock classes
// *****************************************************************************
// /////////////////////////////////////////////////////////////////////////////

class HandlerMock : public SimpleGraph::OpHandler {
    public:
        void handleOperation(const SimpleGraph::VertexAddOperation& op) {
        }
        void handleOperation(const SimpleGraph::VertexRemoveOperation& op) {
        }
        void handleOperation(const SimpleGraph::EdgeAddOperation& op) {
        }
        void handleOperation(const SimpleGraph::EdgeRemoveOperation& op) {
        }
        void handleOperation(const SimpleGraph::AttributeAddOperation& op) {
        }
        void handleOperation(const SimpleGraph::AttributeRemoveOperation& op) {
        }
        void handleOperation(const SimpleGraph::AttributeSetOperation& op) {
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


// TODO


} // End namespace


