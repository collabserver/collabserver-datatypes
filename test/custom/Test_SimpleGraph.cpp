#include "collabdata/custom/SimpleGraph.h"

#include <gtest/gtest.h>

#include "collabdata/custom/OperationObserver.h"


namespace collab {


class SimpleGraphObserver : virtual public OperationObserver {
    public:
        void onOperation(const Operation& op) {
            // TODO
        }
};


TEST(SimpleGraph, constructorTest) {
    SimpleGraph v;
    // TODO
}

TEST(SimpleGraph, addVertexTest) {
    SimpleGraph v;
    v.addVertex("e1");
    // TODO
}

TEST(SimpleGraph, removeVertexTest) {
    SimpleGraph v;
    v.addVertex("e1");
    v.removeVertex("e1");
    // TODO
}

TEST(SimpleGraph, addEdgeTest) {
    SimpleGraph v;
    v.addEdge("e1", "e2");
    // TODO
}

TEST(SimpleGraph, removeEdgeTest) {
    SimpleGraph v;
    v.addEdge("e1", "e2");
    v.removeEdge("e1", "e2");
    // TODO
}

TEST(SimpleGraph, addAttributeTest) {
    SimpleGraph v;
    v.addAttribute("e1", "attr1", "Hi coco!");
    // TODO
}

TEST(SimpleGraph, removeAttributeTest) {
    SimpleGraph v;
    v.addAttribute("e1", "attr1", "Hi coco!");
    v.removeAttribute("e1", "attr1");
    // TODO
}

TEST(SimpleGraph, setAttributeTest) {
    SimpleGraph v;
    v.addAttribute("e1", "attr1", "Hi coco!");
    v.setAttribute("e1", "attr1", "The new coco!");
    // TODO
}


} // End namespace


