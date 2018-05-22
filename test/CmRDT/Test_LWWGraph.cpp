#include "CmRDT/LWWGraph.h"
#include <gtest/gtest.h>

namespace CRDT {
namespace CmRDT {


// -----------------------------------------------------------------------------
// CRDT methods tests
// -----------------------------------------------------------------------------

TEST(LWWGraph, queryVertexTest) {
    LWWGraph<int, int, int> data0;

    // Query before exists
    auto res = data0.queryVertex(1);
    EXPECT_TRUE(res == data0.lend());

    // Add vertex
    data0.addVertex(1, 10);
    res = data0.queryVertex(1);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 1);
    EXPECT_FALSE(res->second._isRemoved);

    // Remove element. Query still works
    data0.removeVertex(1, 20);
    res = data0.queryVertex(1);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 1);
    EXPECT_TRUE(res->second._isRemoved);

    // Query another element before even added
    res = data0.queryVertex(2);
    EXPECT_TRUE(res == data0.lend());

    // Add this element
    data0.addVertex(2, 30);
    res = data0.queryVertex(2);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 2);
    EXPECT_FALSE(res->second._isRemoved);
}

TEST(LWWGraph, addVertexTest) {
    LWWGraph<int, int, int> data0;

    // Add some vertex
    data0.addVertex(0, 10);
    data0.addVertex(1, 11);
    data0.addVertex(2, 12);
    data0.addVertex(3, 13);
    for(int k = 0; k < 4; ++k) {
        auto res = data0.queryVertex(k);
        EXPECT_TRUE(res != data0.lend());
        EXPECT_EQ(res->first, k);
        EXPECT_FALSE(res->second._isRemoved);
        EXPECT_EQ(res->second._timestamp, (10+k));
    }

    // Add duplicate (With various timestamps)
    data0.addVertex(0, 0);
    data0.addVertex(1, 21);
    data0.addVertex(2, 2);
    data0.addVertex(3, 23);
    for(int k = 0; k < 4; ++k) {
        auto res = data0.queryVertex(k);
        EXPECT_TRUE(res != data0.lend());
        EXPECT_EQ(res->first, k);
        EXPECT_FALSE(res->second._isRemoved);
    }

    // Add duplicate, check timestamps is the last value.
    data0.addVertex(4, 38);
    data0.addVertex(4, 33);
    data0.addVertex(4, 39);
    data0.addVertex(4, 32);
    data0.addVertex(4, 33);
    auto res = data0.queryVertex(4);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 4);
    EXPECT_FALSE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 39);
}

TEST(LWWGraph, removeVertexTest) {
    EXPECT_TRUE(false) << "TODO: Test is not implemented yet";
}

TEST(LWWGraph, addEdgeTest) {
    EXPECT_TRUE(false) << "TODO: Test is not implemented yet";
}

TEST(LWWGraph, removeEdgeTest) {
    EXPECT_TRUE(false) << "TODO: Test is not implemented yet";
}

// -----------------------------------------------------------------------------
// Operator tests
// -----------------------------------------------------------------------------

TEST(LWWGraph, operatorEQTest) {
    EXPECT_TRUE(false) << "TODO: Test is not implemented yet";
}

// -----------------------------------------------------------------------------
// Use Case Tests
// -----------------------------------------------------------------------------

TEST(LWWGraph, usecaseGeneralTest) {
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


