#include "collabdata/custom/SimpleGraph.h"

#include <gtest/gtest.h>

#include "collabdata/custom/OperationObserver.h"


namespace collab {


// /////////////////////////////////////////////////////////////////////////////
// *****************************************************************************
// Mock classes
// *****************************************************************************
// /////////////////////////////////////////////////////////////////////////////

class SimpleGraphEventsMock : virtual public SimpleGraph::OperationEvents {
    public:
        SimpleGraphEventsMock() = default;
        ~SimpleGraphEventsMock() = default;

    public:
        void onOperation(const SimpleGraph::VertexAddOperation& op) {
        }
        void onOperation(const SimpleGraph::VertexRemoveOperation& op) {
        }
        void onOperation(const SimpleGraph::EdgeAddOperation& op) {
        }
        void onOperation(const SimpleGraph::EdgeRemoveOperation& op) {
        }
        void onOperation(const SimpleGraph::AttributeAddOperation& op) {
        }
        void onOperation(const SimpleGraph::AttributeRemoveOperation& op) {
        }
        void onOperation(const SimpleGraph::AttributeSetOperation& op) {
        }
};

class SimpleGraphObserverMock : virtual public OperationObserver {
    private:
        SimpleGraphEventsMock _event;
    public:
        void onOperation(const Operation& op) {
            op.accept(_event);
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


