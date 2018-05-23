#include "CmRDT/LWWGraph.h"
#include <gtest/gtest.h>
#include <string>

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
    EXPECT_EQ(res->second._timestamp, 10);

    // Remove element. Query still works
    data0.removeVertex(1, 20);
    res = data0.queryVertex(1);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 1);
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 20);

    // Query another element before even added
    res = data0.queryVertex(2);
    EXPECT_TRUE(res == data0.lend());

    // Add this element
    data0.addVertex(2, 30);
    res = data0.queryVertex(2);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 2);
    EXPECT_FALSE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 30);
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

TEST(LWWGraph, addVertexDuplicateCallsTest) {
    LWWGraph<int, int, int> data0;

    // Add duplicate, check timestamps is the last value.
    data0.addVertex(1, 18);
    data0.addVertex(1, 13);
    data0.addVertex(1, 19);
    data0.addVertex(1, 12);
    data0.addVertex(1, 13);
    auto res = data0.queryVertex(1);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 1);
    EXPECT_FALSE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 19);

    // Some more duplicate on another key
    data0.addVertex(42, 29);
    data0.addVertex(42, 21);
    data0.addVertex(42, 23);
    data0.addVertex(42, 24);
    data0.addVertex(42, 27);
    res = data0.queryVertex(42);
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, 42);
    EXPECT_FALSE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 29);
}

TEST(LWWGraph, removeVertexTest) {
    LWWGraph<std::string, int, int> data0;

    // Add a vertex and remove it
    data0.addVertex("v1", 11);
    data0.removeVertex("v1", 12);
    auto res = data0.queryVertex("v1");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "v1");
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 12);

    // Add a vertex with edges
    data0.addVertex("v1", 20);
    data0.addVertex("v2", 21);
    data0.addVertex("v3", 23);
    data0.addEdge("v2", "v3", 24);
    data0.addEdge("v3", "v2", 25);
    data0.addEdge("v2", "v2", 26);
    data0.addEdge("v3", "v3", 27);
    data0.addEdge("v1", "v2", 28);

    // Remove vertex, should also remove all edges
    data0.removeVertex("v2", 29);
    res = data0.queryVertex("v2");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "v2");
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 29);
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        auto& edges = it->second._content._edges;
        auto edge_it = edges.query("v2");
        bool isRemoved = edge_it->second._isRemoved;
        int timestamp = edge_it->second._timestamp;

        ASSERT_TRUE(edge_it != edges.lend()); // They all add edge with v2
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 29);
    }
}

TEST(LWWGraph, removeVertexBeforeVertexCreatedTest) {
    LWWGraph<std::string, int, int> data0;

    // Remove vertex before even added (Remove it anyway)
    data0.removeVertex("v1", 20);
    auto res = data0.queryVertex("v1");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "v1");
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 20);

    // Receive the add, but too late (Vertex is still the same)
    data0.addVertex("v1", 10);
    res = data0.queryVertex("v1");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "v1");
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 20);
}

TEST(LWWGraph, removeVertexDuplicateCallsTest) {
    LWWGraph<std::string, int, int> data0;

    // Add, then remove vertex
    data0.addVertex("v1", 10);
    data0.removeVertex("v1", 11);
    auto res = data0.queryVertex("v1");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "v1");
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 11);

    // Duplicate remove calls
    data0.removeVertex("v1", 22);
    data0.removeVertex("v1", 28);
    data0.removeVertex("v1", 29);
    data0.removeVertex("v1", 21);
    data0.removeVertex("v1", 27);
    data0.removeVertex("v1", 24);
    res = data0.queryVertex("v1");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "v1");
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 29);
}

TEST(LWWGraph, removeVertexWithEdgesDuplicateCallsTest) {
    LWWGraph<std::string, int, int> data0;

    // Create graph
    data0.addVertex("v1", 11);
    data0.addVertex("v2", 12);
    data0.addVertex("v3", 13);
    data0.addVertex("v4", 14);

    data0.addEdge("v1", "v1", 21);
    data0.addEdge("v1", "v2", 22);
    data0.addEdge("v1", "v3", 23);
    data0.addEdge("v1", "v4", 24);

    data0.addEdge("v2", "v1", 25);
    data0.addEdge("v2", "v3", 26);

    data0.addEdge("v3", "v4", 27);
    data0.addEdge("v3", "v1", 28);

    data0.removeVertex("v1", 30);

    // Remove vertex, should also remove all edges
    auto res = data0.queryVertex("v1");
    EXPECT_TRUE(res != data0.lend());
    EXPECT_EQ(res->first, "v1");
    EXPECT_TRUE(res->second._isRemoved);
    EXPECT_EQ(res->second._timestamp, 30);
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        auto& edges = it->second._content._edges;
        auto edge_it = edges.query("v1");
        bool isRemoved = edge_it->second._isRemoved;
        int timestamp = edge_it->second._timestamp;

        ASSERT_TRUE(edge_it != edges.lend()); // They all add edge with v1
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 30);
    }
}

TEST(LWWGraph, addEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.addVertex("v1", 11);
    data0.addVertex("v2", 12);
    data0.addEdge("v1", "v2", 13);

    // Vertex 1 is 're-added' by addEdge (So timestamps == addEdge timestamps)
    auto v1 = data0.queryVertex("v1");
    EXPECT_TRUE(v1 != data0.lend());
    EXPECT_EQ(v1->first, "v1");
    EXPECT_EQ(v1->second._timestamp, 13);
    EXPECT_FALSE(v1->second._isRemoved);

    // Vertex 2 is 're-added' by addEdge (So timestamps == addEdge timestamps)
    auto v2 = data0.queryVertex("v2");
    EXPECT_TRUE(v2 != data0.lend());
    EXPECT_EQ(v2->first, "v2");
    EXPECT_EQ(v2->second._timestamp, 13);
    EXPECT_FALSE(v2->second._isRemoved);

    // Edge v1 -> v2 should have been created
    auto edges = v1->second._content._edges;
    auto edge = edges.query("v2");
    EXPECT_TRUE(edge != edges.lend());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second._timestamp, 13);
    EXPECT_FALSE(edge->second._isRemoved);
}

TEST(LWWGraph, addEdgeBeforeVertexCreatedTest) {
    LWWGraph<std::string, int, int> data0;

    data0.addEdge("v1", "v2", 10);

    // Vertex 1 should have been created
    auto v1 = data0.queryVertex("v1");
    EXPECT_TRUE(v1 != data0.lend());
    EXPECT_EQ(v1->first, "v1");
    EXPECT_EQ(v1->second._timestamp, 10);
    EXPECT_FALSE(v1->second._isRemoved);

    // Vertex 2 should have been created
    auto v2 = data0.queryVertex("v2");
    EXPECT_TRUE(v2 != data0.lend());
    EXPECT_EQ(v2->first, "v2");
    EXPECT_EQ(v2->second._timestamp, 10);
    EXPECT_FALSE(v2->second._isRemoved);

    // Edge v1 -> v2 should have been created and marked as removed
    auto edges = v1->second._content._edges;
    auto edge = edges.query("v2"); // LWWSet. Returns const_load_iterator
    EXPECT_TRUE(edge != edges.lend());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second._timestamp, 10);
    EXPECT_FALSE(edge->second._isRemoved);


    // Now receive the actual 'addVertex'. (But addEdge created earlier stamp)
    data0.addVertex("v1", 1);
    data0.addVertex("v2", 2);

    v1 = data0.queryVertex("v1");
    v2 = data0.queryVertex("v2");
    edges = v1->second._content._edges;
    edge = edges.query("v2");
    EXPECT_EQ(v1->second._timestamp, 10);
    EXPECT_EQ(v2->second._timestamp, 10);
    EXPECT_EQ(edge->second._timestamp, 10);
}

TEST(LWWGraph, addEdgeDuplicateCallsTest) {
    LWWGraph<std::string, int, int> data0;

    // Received many duplicate addEdge calls
    data0.addEdge("v1", "v2", 10);
    data0.addEdge("v1", "v2", 17);
    data0.addEdge("v1", "v2", 19); // Winner
    data0.addEdge("v1", "v2", 13);
    data0.addEdge("v1", "v2", 11);
    data0.addEdge("v1", "v2", 12);
    data0.addEdge("v1", "v2", 18);

    // Vertex 1
    auto v1 = data0.queryVertex("v1");
    EXPECT_TRUE(v1 != data0.lend());
    EXPECT_EQ(v1->first, "v1");
    EXPECT_EQ(v1->second._timestamp, 19);
    EXPECT_FALSE(v1->second._isRemoved);

    // Vertex 2
    auto v2 = data0.queryVertex("v2");
    EXPECT_TRUE(v2 != data0.lend());
    EXPECT_EQ(v2->first, "v2");
    EXPECT_EQ(v2->second._timestamp, 19);
    EXPECT_FALSE(v2->second._isRemoved);

    // Edge v1 -> v2
    auto edges = v1->second._content._edges;
    auto edge = edges.query("v2");
    EXPECT_TRUE(edge != edges.lend());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second._timestamp, 19);
    EXPECT_FALSE(edge->second._isRemoved);
}

TEST(LWWGraph, removeEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    // Add vertex + edge
    data0.addVertex("v1", 11);
    data0.addVertex("v2", 12);
    data0.removeEdge("v1", "v2", 13);

    // Vertex 1 stay the same
    auto v1 = data0.queryVertex("v1");
    EXPECT_TRUE(v1 != data0.lend());
    EXPECT_EQ(v1->first, "v1");
    EXPECT_EQ(v1->second._timestamp, 11);
    EXPECT_FALSE(v1->second._isRemoved);

    // Vertex 2 stay the same
    auto v2 = data0.queryVertex("v2");
    EXPECT_TRUE(v2 != data0.lend());
    EXPECT_EQ(v2->first, "v2");
    EXPECT_EQ(v2->second._timestamp, 12);
    EXPECT_FALSE(v2->second._isRemoved);

    // Edge v1 -> v2 should have been removed
    auto edges = v1->second._content._edges;
    auto edge = edges.query("v2");
    EXPECT_TRUE(edge != edges.lend());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second._timestamp, 13);
    EXPECT_TRUE(edge->second._isRemoved);
}

TEST(LWWGraph, removeEdgeBeforeAddedTest) {
    LWWGraph<std::string, int, int> data0;

    // Remove edge before added. Two tmp vertex are added.
    data0.removeEdge("v1", "v2", 10);

    // vertex 1 should have been created (with tmp timestamps)
    auto v1 = data0.queryVertex("v1");
    EXPECT_TRUE(v1 != data0.lend());
    EXPECT_EQ(v1->first, "v1");
    EXPECT_EQ(v1->second._timestamp, 0); // Created with smallest timestamp.
    EXPECT_TRUE(v1->second._isRemoved);

    // vertex 2 should have been created (with tmp timestamps)
    auto v2 = data0.queryVertex("v2");
    EXPECT_TRUE(v2 != data0.lend());
    EXPECT_EQ(v2->first, "v2");
    EXPECT_EQ(v2->second._timestamp, 0); // Created with smallest timestamp.
    EXPECT_TRUE(v2->second._isRemoved);

    // Edge v1 -> v2 should have been created and marked as removed
    auto edges = v1->second._content._edges;
    auto edge = edges.query("v2"); // LWWSet. Returns const_load_iterator
    EXPECT_TRUE(edge != edges.lend());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second._timestamp, 10);
    EXPECT_TRUE(edge->second._isRemoved);


    // Duplicate remove calls
    // All timestamps stay same (v1, v2, edge)
    data0.removeEdge("v1", "v2", 2);
    data0.removeEdge("v1", "v2", 1);
    data0.removeEdge("v1", "v2", 3);
    v1 = data0.queryVertex("v1");
    v2 = data0.queryVertex("v2");
    edges = v1->second._content._edges;
    edge = edges.query("v2");
    EXPECT_EQ(v1->second._timestamp, 0);
    EXPECT_EQ(v2->second._timestamp, 0);
    EXPECT_EQ(edge->second._timestamp, 10);


    // Duplicate remove calls with higher timestamps
    // Vertex still have same timestamps. Edge is updated.
    data0.removeEdge("v1", "v2", 24);
    data0.removeEdge("v1", "v2", 20);
    data0.removeEdge("v1", "v2", 29);
    data0.removeEdge("v1", "v2", 28);
    v1 = data0.queryVertex("v1");
    v2 = data0.queryVertex("v2");
    edges = v1->second._content._edges;
    edge = edges.query("v2");
    EXPECT_EQ(v1->second._timestamp, 0);
    EXPECT_EQ(v2->second._timestamp, 0);
    EXPECT_EQ(edge->second._timestamp, 29);
}


// -----------------------------------------------------------------------------
// Operator tests
// -----------------------------------------------------------------------------

TEST(LWWGraph, operatorEQTest) {
    LWWGraph<std::string, int, int> data0; // Data at replicate 0
    LWWGraph<std::string, int, int> data1; // Data at replicate 1


    // Replicate 0
    data0.addVertex("v1", 1);
    data0.addVertex("v2", 2);
    data0.addVertex("v3", 3);
    data0.addVertex("v4", 4);
    data0.addVertex("v5", 5);
    data0.addVertex("v6", 6);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);

    data0.addEdge("v1", "v3", 7);
    data0.addEdge("v2", "v1", 8);
    data0.addEdge("v2", "v4", 9);
    data0.addEdge("v3", "v2", 10);
    data0.addEdge("v4", "v5", 11);
    data0.addEdge("v6", "v3", 12);
    data0.addEdge("v6", "v4", 13);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);

    data0.removeEdge("v6", "v3", 14);
    data0.removeEdge("v6", "v4", 15);
    data0.removeEdge("v3", "v2", 16);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);


    // Replicate 1
    data1.addVertex("v1", 1);
    data1.addVertex("v2", 2);
    data1.addVertex("v3", 3);
    data1.addVertex("v4", 4);
    data1.addVertex("v5", 5);
    data1.addVertex("v6", 6);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);

    data1.addEdge("v1", "v3", 7);
    data1.addEdge("v2", "v1", 8);
    data1.addEdge("v2", "v4", 9);
    data1.addEdge("v3", "v2", 10);
    data1.addEdge("v4", "v5", 11);
    data1.addEdge("v6", "v3", 12);
    data1.addEdge("v6", "v4", 13);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);

    data1.removeEdge("v6", "v3", 14);
    data1.removeEdge("v6", "v4", 15);
    data1.removeEdge("v3", "v2", 16);

    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);
}


// -----------------------------------------------------------------------------
// Use Case Tests
// -----------------------------------------------------------------------------

TEST(LWWGraph, addEdgeRemoveVertexConcurrentTest) {
    LWWGraph<std::string, int, int> data0;

    // Someone create a vertex and delete it.
    data0.addVertex("v1", 10);
    data0.removeVertex("v1", 100);

    // Someone added the same vertex with edge (But earlier in time).
    data0.addVertex("v1", 10);
    data0.addVertex("v2", 11);
    data0.addVertex("v3", 13);
    data0.addEdge("v1", "v2", 21);
    data0.addEdge("v1", "v3", 22);
    data0.addEdge("v2", "v1", 23);
    data0.addEdge("v2", "v3", 24);
    data0.addEdge("v3", "v1", 25);

    // Remove timestamp is last, addEdge must remove edges linked with v1
    // Even if addEdge is called after remove (ex: network latency)

    // V1 is deleted as well as all its edges
    auto v1 = data0.queryVertex("v1");
    EXPECT_TRUE(v1 != data0.lend());
    EXPECT_EQ(v1->first, "v1");
    EXPECT_EQ(v1->second._timestamp, 100);
    EXPECT_TRUE(v1->second._isRemoved);
    for(auto it = data0.lbegin(); it != data0.lend(); ++it) {
        auto& edges = it->second._content._edges;
        auto edge_it = edges.query("v1");
        bool isRemoved = edge_it->second._isRemoved;
        int timestamp = edge_it->second._timestamp;

        ASSERT_TRUE(edge_it != edges.lend()); // They all add edge with v1
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 100);
    }
}


}} // End namespaces


