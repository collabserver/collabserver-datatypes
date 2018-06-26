#include "collabdata/custom/SimpleGraph.h"

#include <gtest/gtest.h>

#include "collabdata/custom/OperationObserver.h"


namespace collab {


class SimpleGraphObserver : virtual public OperationObserver {
    public:
        void notifyOperation(const Operation& op) {
            // TODO
        }
};


TEST(SimpleGraph, constructorTest) {
    SimpleGraph v;
    // TODO
}

TEST(SimpleGraph, addElementTest) {
    SimpleGraph v;
    v.addElement("e1");
    // TODO
}

TEST(SimpleGraph, removeElementTest) {
    SimpleGraph v;
    v.addElement("e1");
    v.removeElement("e1");
    // TODO
}


} // End namespace


