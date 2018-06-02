#include "collab/CmRDT/LWWGraph.h"

#include <gtest/gtest.h>
#include <string>


// Check the whole internal data of a vertex
#define _ASSERT_VERTEX_EQ(vertex_it, key_, is_removed, stamp_, data_) \
    EXPECT_TRUE(vertex_it != data_.crdt_end()); \
    EXPECT_EQ(vertex_it->first, key_); \
    EXPECT_EQ(vertex_it->second.isRemoved(), is_removed); \
    EXPECT_EQ(vertex_it->second.timestamp(), stamp_)

namespace collab {
namespace CmRDT {


// -----------------------------------------------------------------------------
// empty()
// -----------------------------------------------------------------------------

TEST(LWWGraph, emptyTest) {
    LWWGraph<std::string, int, int> data0;
    ASSERT_TRUE(data0.empty());

    data0.addVertex("v1", 10);
    ASSERT_FALSE(data0.empty());

    data0.removeVertex("v1", 20);
    ASSERT_TRUE(data0.empty());

    // Try to add but smaller timestamp: still emtpy
    data0.addVertex("v1", 11);
    data0.addVertex("v1", 12);
    ASSERT_TRUE(data0.empty());

    data0.addVertex("v2", 30);
    ASSERT_FALSE(data0.empty());
}


// -----------------------------------------------------------------------------
// size()
// -----------------------------------------------------------------------------

TEST(LWWGraph, sizeTest) {
    LWWGraph<std::string, int, int> data0;
    ASSERT_EQ(data0.size(), 0);

    // Add vertex
    data0.addVertex("v1", 10);
    ASSERT_EQ(data0.size(), 1);
    data0.addVertex("v2", 20);
    ASSERT_EQ(data0.size(), 2);
    data0.addVertex("v3", 30);
    ASSERT_EQ(data0.size(), 3);
    data0.addVertex("v4", 40);
    ASSERT_EQ(data0.size(), 4);

    // Remove vertex
    data0.removeVertex("v1", 50);
    ASSERT_EQ(data0.size(), 3);
    data0.removeVertex("v2", 60);
    ASSERT_EQ(data0.size(), 2);
    data0.removeVertex("v3", 70);
    ASSERT_EQ(data0.size(), 1);
    data0.removeVertex("v4", 80);
    ASSERT_EQ(data0.size(), 0);
}

TEST(LWWGraph, sizeWithDuplicateAddTest) {
    LWWGraph<int, int, int> data0;

    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.size(), 0);

    data0.addVertex(1, 10);
    data0.addVertex(1, 18);
    data0.addVertex(1, 19);
    data0.addVertex(1, 11);
    data0.addVertex(1, 15);
    ASSERT_EQ(data0.size(), 1);
}

TEST(LWWGraph, sizeWithAddEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.addEdge("v1", "v1", 10);
    ASSERT_EQ(data0.size(), 1);
    data0.addEdge("v1", "v2", 20);
    ASSERT_EQ(data0.size(), 2);
}

TEST(LWWGraph, sizeWithRemoveEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.removeEdge("v1", "v1", 10);
    ASSERT_EQ(data0.size(), 0);
    data0.removeEdge("v1", "v2", 20);
    ASSERT_EQ(data0.size(), 0);
}

TEST(LWWGraph, sizeWithAddEdgeRemoveEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.addEdge("v1", "v2", 10);
    data0.addEdge("v1", "v3", 20);
    data0.removeEdge("v1", "v2", 30);
    ASSERT_EQ(data0.size(), 3);
}

TEST(LWWGraph, sizeWithAddEdgeRemoveVertex) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // This illustrate the scenario of concurrent addEdge || removeVertex

    // init data0
    data0.addVertex("v1", 110);
    data0.addVertex("v2", 120);
    data0.addEdge("v1", "v2", 130);

    // init data1
    data1.addVertex("v1", 110);
    data1.addVertex("v2", 120);
    data1.addEdge("v1", "v2", 130);

    // data0 apply addEdge first
    data0.addEdge("v2", "v3", 140);
    data0.removeVertex("v1", 150);

    // data1 apply removeVertex first
    data1.removeVertex("v1", 150);
    data1.addEdge("v2", "v3", 140);

    ASSERT_EQ(data0.size(), 2);
    ASSERT_EQ(data1.size(), 2);
    ASSERT_EQ(data0.size(), data1.size());
}


// -----------------------------------------------------------------------------
// query()
// -----------------------------------------------------------------------------

TEST(LWWGraph, queryVertexTest) {
    LWWGraph<int, int, int> data0;

    // Query before exists
    auto res = data0.queryVertex(1);
    EXPECT_TRUE(res == data0.crdt_end());

    // Add vertex
    data0.addVertex(1, 10);
    res = data0.queryVertex(1);
    _ASSERT_VERTEX_EQ(res, 1, false, 10, data0);

    // Remove element. Query still works
    data0.removeVertex(1, 20);
    res = data0.queryVertex(1);
    _ASSERT_VERTEX_EQ(res, 1, true, 20, data0);

    // Query another element before even added
    res = data0.queryVertex(2);
    EXPECT_TRUE(res == data0.crdt_end());

    // Add this element
    data0.addVertex(2, 30);
    res = data0.queryVertex(2);
    _ASSERT_VERTEX_EQ(res, 2, false, 30, data0);
}


// -----------------------------------------------------------------------------
// addVertex()
// -----------------------------------------------------------------------------

TEST(LWWGraph, addVertexTest) {
    LWWGraph<int, int, int> data0;

    // Add some vertex
    data0.addVertex(0, 10);
    data0.addVertex(1, 11);
    data0.addVertex(2, 12);
    data0.addVertex(3, 13);
    for(int k = 0; k < 4; ++k) {
        auto res = data0.queryVertex(k);
        _ASSERT_VERTEX_EQ(res, k, false, (10+k), data0);
    }

    // Add duplicate (With various timestamps but always inferior)
    data0.addVertex(0, 0);
    data0.addVertex(1, 9);
    data0.addVertex(2, 2);
    data0.addVertex(3, 7);
    for(int k = 0; k < 4; ++k) {
        auto res = data0.queryVertex(k);
        _ASSERT_VERTEX_EQ(res, k, false, (10+k), data0);
    }

    // Add duplicate, check timestamps is the last value.
    data0.addVertex(4, 38);
    data0.addVertex(4, 33);
    data0.addVertex(4, 39);
    data0.addVertex(4, 32);
    data0.addVertex(4, 33);
    auto res = data0.queryVertex(4);
    _ASSERT_VERTEX_EQ(res, 4, false, 39, data0);
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
    _ASSERT_VERTEX_EQ(res, 1, false, 19, data0);

    // Some more duplicate on another key
    data0.addVertex(42, 29);
    data0.addVertex(42, 21);
    data0.addVertex(42, 23);
    data0.addVertex(42, 24);
    data0.addVertex(42, 27);
    res = data0.queryVertex(42);
    _ASSERT_VERTEX_EQ(res, 42, false, 29, data0);
}


// -----------------------------------------------------------------------------
// removeVertex()
// -----------------------------------------------------------------------------

TEST(LWWGraph, removeVertexTest) {
    LWWGraph<std::string, int, int> data0;

    // Add a vertex and remove it
    data0.addVertex("v1", 11);
    data0.removeVertex("v1", 12);
    auto res = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 12, data0);

    // Add a vertex with edges
    data0.addVertex("v1", 20);
    data0.addVertex("v2", 21);
    data0.addVertex("v3", 23);
    data0.addEdge("v2", "v3", 24);
    data0.addEdge("v3", "v2", 25);
    data0.addEdge("v2", "v2", 26);
    data0.addEdge("v3", "v3", 27);
    data0.addEdge("v1", "v2", 28);

    // Remove vertex
    data0.removeVertex("v2", 29);
    res = data0.queryVertex("v2");
    _ASSERT_VERTEX_EQ(res, "v2", true, 29, data0);

    // All edges with v2 (to and from) should be removed.
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        auto& edges = it->second.value().edges();
        auto edge_it = edges.query("v2");
        bool isRemoved = edge_it->second.isRemoved();
        int timestamp = edge_it->second.timestamp();

        ASSERT_TRUE(edge_it != edges.crdt_end()); // They all add edge with v2
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 29);
    }
}

TEST(LWWGraph, removeVertexBeforeVertexCreatedTest) {
    LWWGraph<std::string, int, int> data0;

    // Remove vertex before even added (Remove it anyway)
    data0.removeVertex("v1", 20);
    auto res = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 20, data0);

    // Receive the add, but too late (Vertex is still the same)
    data0.addVertex("v1", 10);
    res = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 20, data0);
}

TEST(LWWGraph, removeVertexDuplicateCallsTest) {
    LWWGraph<std::string, int, int> data0;

    // Add, then remove vertex
    data0.addVertex("v1", 10);
    data0.removeVertex("v1", 11);
    auto res = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 11, data0);

    // Duplicate remove calls
    data0.removeVertex("v1", 22);
    data0.removeVertex("v1", 28);
    data0.removeVertex("v1", 29);
    data0.removeVertex("v1", 21);
    data0.removeVertex("v1", 27);
    data0.removeVertex("v1", 24);
    res = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 29, data0);
}


// -----------------------------------------------------------------------------
// removeVertex() || addEdge()
// -----------------------------------------------------------------------------

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
    _ASSERT_VERTEX_EQ(res, "v1", true, 30, data0);
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        auto& edges = it->second.value().edges();
        auto edge_it = edges.query("v1");
        bool isRemoved = edge_it->second.isRemoved();
        int timestamp = edge_it->second.timestamp();

        ASSERT_TRUE(edge_it != edges.crdt_end()); // They all add edge with v1
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 30);
    }
}


// -----------------------------------------------------------------------------
// addEdge()
// -----------------------------------------------------------------------------

TEST(LWWGraph, addEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.addVertex("v1", 11);
    data0.addVertex("v2", 12);
    data0.addEdge("v1", "v2", 13);

    // Vertex 1 is 're-added' by addEdge (So timestamps == addEdge timestamps)
    auto v1 = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 13, data0);

    // Vertex 2 is 're-added' by addEdge (So timestamps == addEdge timestamps)
    auto v2 = data0.queryVertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", false, 13, data0);

    // Edge v1 -> v2 should have been created
    auto edges = v1->second.value().edges();
    auto edge = edges.query("v2");
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 13);
    EXPECT_FALSE(edge->second.isRemoved());
}

TEST(LWWGraph, addEdgeBeforeVertexCreatedTest) {
    LWWGraph<std::string, int, int> data0;

    data0.addEdge("v1", "v2", 10);

    // Vertex 1 should have been created
    auto v1 = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 10, data0);

    // Vertex 2 should have been created
    auto v2 = data0.queryVertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", false, 10, data0);

    // Edge v1 -> v2 should have been created and marked as removed
    auto edges = v1->second.value().edges();
    auto edge = edges.query("v2"); // LWWSet. Returns const_crdt_iterator
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 10);
    EXPECT_FALSE(edge->second.isRemoved());


    // Now receive the actual 'addVertex'. (But addEdge created earlier stamp)
    data0.addVertex("v1", 1);
    data0.addVertex("v2", 2);

    v1 = data0.queryVertex("v1");
    v2 = data0.queryVertex("v2");
    edges = v1->second.value().edges();
    edge = edges.query("v2");
    EXPECT_EQ(v1->second.timestamp(), 10);
    EXPECT_EQ(v2->second.timestamp(), 10);
    EXPECT_EQ(edge->second.timestamp(), 10);
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
    _ASSERT_VERTEX_EQ(v1, "v1", false, 19, data0);

    // Vertex 2
    auto v2 = data0.queryVertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", false, 19, data0);

    // Edge v1 -> v2
    auto edges = v1->second.value().edges();
    auto edge = edges.query("v2");
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 19);
    EXPECT_FALSE(edge->second.isRemoved());
}


// -----------------------------------------------------------------------------
// removeEdge()
// -----------------------------------------------------------------------------

TEST(LWWGraph, removeEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    // Add vertex + edge
    data0.addVertex("v1", 11);
    data0.addVertex("v2", 12);
    data0.removeEdge("v1", "v2", 13);

    // Vertex 1 stay the same
    auto v1 = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 11, data0);

    // Vertex 2 stay the same
    auto v2 = data0.queryVertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", false, 12, data0);

    // Edge v1 -> v2 should have been removed
    auto edges = v1->second.value().edges();
    auto edge = edges.query("v2");
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 13);
    EXPECT_TRUE(edge->second.isRemoved());
}

TEST(LWWGraph, removeEdgeBeforeAddedTest) {
    LWWGraph<std::string, int, int> data0;

    // Remove edge before added. Two tmp vertex are added.
    data0.removeEdge("v1", "v2", 10);

    // vertex 1 should have been created (with tmp timestamps)
    auto v1 = data0.queryVertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", true, 0, data0); // Smallest timestamp.

    // vertex 2 should have been created (with tmp timestamps)
    auto v2 = data0.queryVertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", true, 0, data0); // Smallest timestamp.

    // Edge v1 -> v2 should have been created and marked as removed
    auto edges = v1->second.value().edges();
    auto edge = edges.query("v2"); // LWWSet. Returns const_crdt_iterator
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 10);
    EXPECT_TRUE(edge->second.isRemoved());


    // Duplicate remove calls
    // All timestamps stay same (v1, v2, edge)
    data0.removeEdge("v1", "v2", 2);
    data0.removeEdge("v1", "v2", 1);
    data0.removeEdge("v1", "v2", 3);
    v1 = data0.queryVertex("v1");
    v2 = data0.queryVertex("v2");
    edges = v1->second.value().edges();
    edge = edges.query("v2");
    EXPECT_EQ(v1->second.timestamp(), 0);
    EXPECT_EQ(v2->second.timestamp(), 0);
    EXPECT_EQ(edge->second.timestamp(), 10);


    // Duplicate remove calls with higher timestamps
    // Vertex still have same timestamps. Edge is updated.
    data0.removeEdge("v1", "v2", 24);
    data0.removeEdge("v1", "v2", 20);
    data0.removeEdge("v1", "v2", 29);
    data0.removeEdge("v1", "v2", 28);
    v1 = data0.queryVertex("v1");
    v2 = data0.queryVertex("v2");
    edges = v1->second.value().edges();
    edge = edges.query("v2");
    EXPECT_EQ(v1->second.timestamp(), 0);
    EXPECT_EQ(v2->second.timestamp(), 0);
    EXPECT_EQ(edge->second.timestamp(), 29);
}


// -----------------------------------------------------------------------------
// addEdge() || removeVertex()
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
    EXPECT_TRUE(v1 != data0.crdt_end());
    EXPECT_EQ(v1->first, "v1");
    EXPECT_EQ(v1->second.timestamp(), 100);
    EXPECT_TRUE(v1->second.isRemoved());
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        auto& edges = it->second.value().edges();
        auto edge_it = edges.query("v1");
        bool isRemoved = edge_it->second.isRemoved();
        int timestamp = edge_it->second.timestamp();

        ASSERT_TRUE(edge_it != edges.crdt_end()); // They all add edge with v1
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 100);
    }
}


// -----------------------------------------------------------------------------
// crdt_size()
// -----------------------------------------------------------------------------

TEST(LWWGraph, crdt_sizeWithAddEdgeRemoveVertex) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // This illustrate the scenario of concurrent addEdge || removeVertex

    // init data0
    data0.addVertex("v1", 110);
    data0.addVertex("v2", 120);
    data0.addEdge("v1", "v2", 130);

    // init data1
    data1.addVertex("v1", 110);
    data1.addVertex("v2", 120);
    data1.addEdge("v1", "v2", 130);

    // data0 apply addEdge first
    data0.addEdge("v2", "v3", 140);
    data0.removeVertex("v1", 150);

    // data1 apply removeVertex first
    data1.removeVertex("v1", 150);
    data1.addEdge("v2", "v3", 140);

    ASSERT_EQ(data0.crdt_size(), 3);
    ASSERT_EQ(data1.crdt_size(), 3);
    ASSERT_EQ(data0.crdt_size(), data1.crdt_size());
}


// -----------------------------------------------------------------------------
// crdt_equal()
// -----------------------------------------------------------------------------

TEST(LWWGraph, crdt_equalWithOnlyAddVertexTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // Replicate data0
    data0.addVertex("v1", 11);
    data0.addVertex("v2", 12);
    data0.addVertex("v3", 13);
    data0.addVertex("v4", 14);
    data0.addVertex("v5", 15);

    // Replicate data1
    data1.addVertex("v6", 21);
    data1.addVertex("v7", 22);
    data1.addVertex("v8", 23);
    data1.addVertex("v9", 24);

    // At this point, data0 != data1
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast data0 to data1
    data1.addVertex("v1", 11);
    data1.addVertex("v2", 12);
    data1.addVertex("v3", 13);
    data1.addVertex("v4", 14);
    data1.addVertex("v5", 15);

    // Broadcast data1 to data0
    data0.addVertex("v6", 21);
    data0.addVertex("v7", 22);
    data0.addVertex("v8", 23);
    data0.addVertex("v9", 24);

    // Now, data0 == data1
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));
}

TEST(LWWGraph, crdt_equalWithAddVertexRemoveVertexTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // Replicate data0
    data0.addVertex("v1", 110);
    data0.addVertex("v2", 120);
    data0.addVertex("v3", 130);
    data0.removeVertex("v1", 140);
    data0.removeVertex("v3", 150);
    data0.addVertex("v1", 150);

    // Replicate data1
    data1.addVertex("v5", 101);
    data1.removeVertex("v5", 111);
    data1.addVertex("v2", 111);
    data1.addVertex("v1", 111);
    data1.addVertex("v5", 121);
    data1.addVertex("v6", 131);
    data1.removeVertex("v1", 201);

    // Atm, data0 != data1
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));


    // Broadcast data0 to data1
    data1.addVertex("v1", 110);
    data1.addVertex("v2", 120);
    data1.addVertex("v3", 130);
    data1.removeVertex("v1", 140);
    data1.removeVertex("v3", 150);
    data1.addVertex("v1", 150);

    // Broadcast data1 to data0
    data0.addVertex("v5", 101);
    data0.removeVertex("v5", 111);
    data0.addVertex("v2", 111);
    data0.addVertex("v1", 111);
    data0.addVertex("v5", 121);
    data0.addVertex("v6", 131);
    data0.removeVertex("v1", 201);

    // Now, data0 == data1
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));
}

TEST(LWWGraph, crdt_equalWithAddEdgeRemoveVertexTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // This illustrate the scenario of concurrent addEdge || removeVertex

    // Replicate data0
    data0.addVertex("v1", 110);
    data0.addVertex("v2", 120);
    data0.addEdge("v1", "v2", 130);
    data0.addEdge("v1", "v3", 200);
    data0.addEdge("v2", "v1", 210);

    // Replicate data1
    data1.addVertex("v1", 111);
    data1.addVertex("v2", 121);
    data1.addEdge("v1", "v2", 131);
    data1.removeVertex("v1", 301);


    // data0 != data1
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast data0 to data1
    data1.addVertex("v1", 110);
    data1.addVertex("v2", 120);
    data1.addEdge("v1", "v2", 130);
    data1.addEdge("v1", "v3", 200);
    data1.addEdge("v2", "v1", 210);

    // Broadcast data1 to data0
    data0.addVertex("v1", 111);
    data0.addVertex("v2", 121);
    data0.addEdge("v1", "v2", 131);
    data0.removeVertex("v1", 301);


    // data0 == data1
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));

    // Just to test, data0 equal data0
    ASSERT_TRUE(data0.crdt_equal(data0));
    ASSERT_TRUE(data1.crdt_equal(data1));
}

TEST(LWWGraph, crdt_equalOnEmptyGraphTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    ASSERT_TRUE(data0.crdt_equal(data0));
    ASSERT_TRUE(data1.crdt_equal(data1));
}

TEST(LWWGraph, crdt_equalEmptyVsAddVertexTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    ASSERT_TRUE(data0.crdt_equal(data0));
    ASSERT_TRUE(data1.crdt_equal(data1));

    // Add crap in data0
    data0.addVertex("v1", 10);
    data0.addVertex("v2", 10);
    data0.addVertex("v3", 10);

    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));
}


// -----------------------------------------------------------------------------
// Operator==()
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


}} // End namespaces


