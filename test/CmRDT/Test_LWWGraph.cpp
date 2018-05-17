#include "CmRDT/LWWGraph.h"
#include <gtest/gtest.h>

namespace CRDT {
namespace CmRDT {


// -----------------------------------------------------------------------------
// CRDT methods tests
// -----------------------------------------------------------------------------

TEST(LWWGraph, queryVertexTest) {
    EXPECT_TRUE(false) << "TODO Test";
}

TEST(LWWGraph, queryEdgeTest) {
    EXPECT_TRUE(false) << "TODO Test";
}

TEST(LWWGraph, addVertexTest) {
    EXPECT_TRUE(false) << "TODO Test";
}

TEST(LWWGraph, removeVertexTest) {
    EXPECT_TRUE(false) << "TODO Test";
}

TEST(LWWGraph, addEdgeTest) {
    EXPECT_TRUE(false) << "TODO Test";
}

TEST(LWWGraph, removeEdgeTest) {
    EXPECT_TRUE(false) << "TODO Test";
}

// -----------------------------------------------------------------------------
// Operator tests
// -----------------------------------------------------------------------------

TEST(LWWGraph, operatorEQTest) {
    EXPECT_TRUE(false) << "TODO Test";
}

// -----------------------------------------------------------------------------
// Use Case Tests
// -----------------------------------------------------------------------------

TEST(LWWGraph, useCaseTest) {
    LWWGraph<std::string, int, int> data0; // Data at replicate 0
    LWWGraph<std::string, int, int> data1; // Data at replicate 1


    // Replicate 0
    data0.addVertex("v1", 1);
    data0.addVertex("v2", 2);
    data0.addVertex("v3", 3);
    data0.addVertex("v4", 4);
    data0.addVertex("v5", 5);
    data0.addVertex("v6", 6);

    data0.addEdge("v1", "v3", 7);
    data0.addEdge("v2", "v1", 8);
    data0.addEdge("v2", "v4", 9);
    data0.addEdge("v3", "v2", 10);
    data0.addEdge("v4", "v5", 11);
    data0.addEdge("v6", "v3", 12);
    data0.addEdge("v6", "v4", 13);

    data0.removeEdge("v6", "v3", 14);
    data0.removeEdge("v6", "v4", 15);
    data0.removeEdge("v3", "v2", 16);


    // Replicate 1
    data1.addVertex("v1", 1);
    data1.addVertex("v2", 2);
    data1.addVertex("v3", 3);
    data1.addVertex("v4", 4);
    data1.addVertex("v5", 5);
    data1.addVertex("v6", 6);

    data1.addEdge("v1", "v3", 7);
    data1.addEdge("v2", "v1", 8);
    data1.addEdge("v2", "v4", 9);
    data1.addEdge("v3", "v2", 10);
    data1.addEdge("v4", "v5", 11);
    data1.addEdge("v6", "v3", 12);
    data1.addEdge("v6", "v4", 13);

    data1.removeEdge("v6", "v3", 14);
    data1.removeEdge("v6", "v4", 15);
    data1.removeEdge("v3", "v2", 16);

    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);
}

}} // End namespaces


