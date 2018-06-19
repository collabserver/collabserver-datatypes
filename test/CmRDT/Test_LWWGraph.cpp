#include "collabdata/CmRDT/LWWGraph.h"

#include <gtest/gtest.h>
#include <string>
#include <iostream>


// Check the whole internal data of a vertex
#define _ASSERT_VERTEX_EQ(vertex_it, key_, is_removed, stamp_, data_) \
    EXPECT_TRUE(vertex_it != data_.crdt_end()); \
    EXPECT_EQ(vertex_it->first, key_); \
    EXPECT_EQ(vertex_it->second.isRemoved(), is_removed); \
    EXPECT_EQ(vertex_it->second.timestamp(), stamp_)

// Check AddEdgeInfo data
#define _ASSERT_ADD_EDGE_INFO_EQ(info_, edge_added_, from_added_, to_added_) \
    ASSERT_EQ(info_.isEdgeAdded, edge_added_); \
    ASSERT_EQ(info_.isFromAdded, from_added_); \
    ASSERT_EQ(info_.isToAdded, to_added_)


namespace collab {
namespace CmRDT {


// -----------------------------------------------------------------------------
// empty()
// -----------------------------------------------------------------------------

TEST(LWWGraph, emptyTest) {
    LWWGraph<std::string, int, int> data0;
    ASSERT_TRUE(data0.empty());

    data0.add_vertex("v1", 10);
    ASSERT_FALSE(data0.empty());

    data0.remove_vertex("v1", 20);
    ASSERT_TRUE(data0.empty());

    // Try to add but smaller timestamp: still emtpy
    data0.add_vertex("v1", 11);
    data0.add_vertex("v1", 12);
    ASSERT_TRUE(data0.empty());

    data0.add_vertex("v2", 30);
    ASSERT_FALSE(data0.empty());
}


// -----------------------------------------------------------------------------
// crdt_empty()
// -----------------------------------------------------------------------------

TEST(LWWGraph, crdt_emptyTest) {
    LWWGraph<std::string, int, int> data0;
    ASSERT_TRUE(data0.crdt_empty());

    data0.add_vertex("v1", 10);
    ASSERT_FALSE(data0.crdt_empty());

    data0.remove_vertex("v1", 20);
    ASSERT_FALSE(data0.crdt_empty());

    // Try to add but smaller timestamp: still emtpy
    data0.add_vertex("v1", 11);
    data0.add_vertex("v1", 12);
    ASSERT_FALSE(data0.crdt_empty());

    data0.add_vertex("v2", 30);
    ASSERT_FALSE(data0.crdt_empty());
}


// -----------------------------------------------------------------------------
// size()
// -----------------------------------------------------------------------------

TEST(LWWGraph, sizeTest) {
    LWWGraph<std::string, int, int> data0;
    ASSERT_EQ(data0.size(), 0);

    // Add vertex
    data0.add_vertex("v1", 10);
    ASSERT_EQ(data0.size(), 1);
    data0.add_vertex("v2", 20);
    ASSERT_EQ(data0.size(), 2);
    data0.add_vertex("v3", 30);
    ASSERT_EQ(data0.size(), 3);
    data0.add_vertex("v4", 40);
    ASSERT_EQ(data0.size(), 4);

    // Remove vertex
    data0.remove_vertex("v1", 50);
    ASSERT_EQ(data0.size(), 3);
    data0.remove_vertex("v2", 60);
    ASSERT_EQ(data0.size(), 2);
    data0.remove_vertex("v3", 70);
    ASSERT_EQ(data0.size(), 1);
    data0.remove_vertex("v4", 80);
    ASSERT_EQ(data0.size(), 0);
}

TEST(LWWGraph, sizeWithDuplicateAddTest) {
    LWWGraph<int, int, int> data0;

    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.size(), 0);

    data0.add_vertex(1, 10);
    data0.add_vertex(1, 18);
    data0.add_vertex(1, 19);
    data0.add_vertex(1, 11);
    data0.add_vertex(1, 15);
    ASSERT_EQ(data0.size(), 1);
}

TEST(LWWGraph, sizeWithAddEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_edge("v1", "v1", 10);
    ASSERT_EQ(data0.size(), 1);
    data0.add_edge("v1", "v2", 20);
    ASSERT_EQ(data0.size(), 2);
}

TEST(LWWGraph, sizeWithRemoveEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.remove_edge("v1", "v1", 10);
    ASSERT_EQ(data0.size(), 0);
    data0.remove_edge("v1", "v2", 20);
    ASSERT_EQ(data0.size(), 0);
}

TEST(LWWGraph, sizeWithAddEdgeRemoveEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_edge("v1", "v2", 10);
    data0.add_edge("v1", "v3", 20);
    data0.remove_edge("v1", "v2", 30);
    ASSERT_EQ(data0.size(), 3);
}

TEST(LWWGraph, sizeWithadd_edgeremove_vertex) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // This illustrate the scenario of concurrent add_edge || remove_vertex

    // init data0
    data0.add_vertex("v1", 110);
    data0.add_vertex("v2", 120);
    data0.add_edge("v1", "v2", 130);

    // init data1
    data1.add_vertex("v1", 110);
    data1.add_vertex("v2", 120);
    data1.add_edge("v1", "v2", 130);

    // data0 apply add_edge first
    data0.add_edge("v2", "v3", 140);
    data0.remove_vertex("v1", 150);

    // data1 apply remove_vertex first
    data1.remove_vertex("v1", 150);
    data1.add_edge("v2", "v3", 140);

    ASSERT_EQ(data0.size(), 2);
    ASSERT_EQ(data1.size(), 2);
    ASSERT_EQ(data0.size(), data1.size());
}


// -----------------------------------------------------------------------------
// crdt_size()
// -----------------------------------------------------------------------------

TEST(LWWGraph, crdt_sizeTest) {
    LWWGraph<std::string, int, int> data0;
    ASSERT_EQ(data0.crdt_size(), 0);

    // Add vertex
    data0.add_vertex("v1", 10);
    ASSERT_EQ(data0.crdt_size(), 1);
    data0.add_vertex("v2", 20);
    ASSERT_EQ(data0.crdt_size(), 2);
    data0.add_vertex("v3", 30);
    ASSERT_EQ(data0.crdt_size(), 3);
    data0.add_vertex("v4", 40);
    ASSERT_EQ(data0.crdt_size(), 4);

    // Remove vertex
    data0.remove_vertex("v1", 50);
    ASSERT_EQ(data0.crdt_size(), 4);
    data0.remove_vertex("v2", 60);
    ASSERT_EQ(data0.crdt_size(), 4);
    data0.remove_vertex("v3", 70);
    ASSERT_EQ(data0.crdt_size(), 4);
    data0.remove_vertex("v4", 80);
    ASSERT_EQ(data0.crdt_size(), 4);
}


// -----------------------------------------------------------------------------
// crdt_find_vertex()
// -----------------------------------------------------------------------------

TEST(LWWGraph, crdt_find_vertexTest) {
    LWWGraph<int, int, int> data0;

    // Query before exists
    auto res = data0.crdt_find_vertex(1);
    EXPECT_TRUE(res == data0.crdt_end());

    // Add vertex
    data0.add_vertex(1, 10);
    res = data0.crdt_find_vertex(1);
    _ASSERT_VERTEX_EQ(res, 1, false, 10, data0);

    // Remove element. Query still works
    data0.remove_vertex(1, 20);
    res = data0.crdt_find_vertex(1);
    _ASSERT_VERTEX_EQ(res, 1, true, 20, data0);

    // Query another element before even added
    res = data0.crdt_find_vertex(2);
    EXPECT_TRUE(res == data0.crdt_end());

    // Add this element
    data0.add_vertex(2, 30);
    res = data0.crdt_find_vertex(2);
    _ASSERT_VERTEX_EQ(res, 2, false, 30, data0);
}


// -----------------------------------------------------------------------------
// count_vertex()
// -----------------------------------------------------------------------------

TEST(LWWGraph, count_vertexTest) {
    LWWGraph<int, int, int> data0;

    // Query before exists
    int res = data0.count_vertex(1);
    EXPECT_EQ(res, 0);

    // Add vertex
    data0.add_vertex(1, 10);
    data0.add_vertex(2, 10);
    data0.add_vertex(3, 10);
    data0.add_vertex(4, 10);
    data0.add_vertex(5, 10);
    data0.remove_vertex(1, 20);
    data0.remove_vertex(2, 20);
    data0.remove_vertex(3, 20);

    res = data0.count_vertex(1);
    EXPECT_EQ(res, 0);
    res = data0.count_vertex(2);
    EXPECT_EQ(res, 0);
    res = data0.count_vertex(3);
    EXPECT_EQ(res, 0);
    res = data0.count_vertex(4);
    EXPECT_EQ(res, 1);
    res = data0.count_vertex(5);
    EXPECT_EQ(res, 1);
}


// -----------------------------------------------------------------------------
// crdt_count_vertex()
// -----------------------------------------------------------------------------

TEST(LWWGraph, crdt_count_vertexTest) {
    LWWGraph<int, int, int> data0;

    // Query before exists
    int res = data0.crdt_count_vertex(1);
    EXPECT_EQ(res, 0);

    // Add vertex
    data0.add_vertex(1, 10);
    data0.add_vertex(2, 10);
    data0.add_vertex(3, 10);
    data0.add_vertex(4, 10);
    data0.add_vertex(5, 10);
    data0.remove_vertex(1, 20);
    data0.remove_vertex(2, 20);
    data0.remove_vertex(3, 20);

    res = data0.crdt_count_vertex(1);
    EXPECT_EQ(res, 1);
    res = data0.crdt_count_vertex(2);
    EXPECT_EQ(res, 1);
    res = data0.crdt_count_vertex(3);
    EXPECT_EQ(res, 1);
    res = data0.crdt_count_vertex(4);
    EXPECT_EQ(res, 1);
    res = data0.crdt_count_vertex(5);
    EXPECT_EQ(res, 1);
}


// -----------------------------------------------------------------------------
// count_edge()
// -----------------------------------------------------------------------------

TEST(LWWGraph, count_edgeTest) {
    LWWGraph<std::string, int, int> data0;

    int res = data0.count_edge("v1", "v2");
    ASSERT_EQ(res, 0);

    // Init data
    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);

    data0.add_edge("v1", "v1", 21);
    data0.add_edge("v1", "v2", 22);
    data0.remove_edge("v1", "v2", 31);

    data0.add_edge("v2", "v1", 24);
    data0.add_edge("v2", "v2", 25);
    data0.add_edge("v2", "v3", 26);
    data0.remove_edge("v2", "v2", 32);

    data0.add_edge("v3", "v2", 27);
    data0.remove_edge("v3", "v2", 33);


    // Check
    int v1v1 = data0.count_edge("v1", "v1");
    int v1v2 = data0.count_edge("v1", "v2");
    int v1v3 = data0.count_edge("v1", "v3");
    ASSERT_EQ(v1v1, 1);
    ASSERT_EQ(v1v2, 0);
    ASSERT_EQ(v1v3, 0);

    int v2v1 = data0.count_edge("v2", "v1");
    int v2v2 = data0.count_edge("v2", "v2");
    int v2v3 = data0.count_edge("v2", "v3");
    ASSERT_EQ(v2v1, 1);
    ASSERT_EQ(v2v2, 0);
    ASSERT_EQ(v2v3, 1);

    int v3v1 = data0.count_edge("v3", "v1");
    int v3v2 = data0.count_edge("v3", "v2");
    int v3v3 = data0.count_edge("v3", "v3");
    ASSERT_EQ(v3v1, 0);
    ASSERT_EQ(v3v2, 0);
    ASSERT_EQ(v3v3, 0);
}


// -----------------------------------------------------------------------------
// crdt_count_edge()
// -----------------------------------------------------------------------------

TEST(LWWGraph, crdt_count_edgeTest) {
    LWWGraph<std::string, int, int> data0;

    int res = data0.crdt_count_edge("v1", "v2");
    ASSERT_EQ(res, 0);

    // Init data
    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);

    data0.add_edge("v1", "v1", 21);
    data0.add_edge("v1", "v2", 22);
    data0.remove_edge("v1", "v2", 31);

    data0.add_edge("v2", "v1", 24);
    data0.add_edge("v2", "v2", 25);
    data0.add_edge("v2", "v3", 26);
    data0.remove_edge("v2", "v2", 32);

    data0.add_edge("v3", "v2", 27);
    data0.remove_edge("v3", "v2", 33);


    // Check
    int v1v1 = data0.crdt_count_edge("v1", "v1");
    int v1v2 = data0.crdt_count_edge("v1", "v2");
    int v1v3 = data0.crdt_count_edge("v1", "v3");
    ASSERT_EQ(v1v1, 1);
    ASSERT_EQ(v1v2, 1);
    ASSERT_EQ(v1v3, 0);

    int v2v1 = data0.crdt_count_edge("v2", "v1");
    int v2v2 = data0.crdt_count_edge("v2", "v2");
    int v2v3 = data0.crdt_count_edge("v2", "v3");
    ASSERT_EQ(v2v1, 1);
    ASSERT_EQ(v2v2, 1);
    ASSERT_EQ(v2v3, 1);

    int v3v1 = data0.crdt_count_edge("v3", "v1");
    int v3v2 = data0.crdt_count_edge("v3", "v2");
    int v3v3 = data0.crdt_count_edge("v3", "v3");
    ASSERT_EQ(v3v1, 0);
    ASSERT_EQ(v3v2, 1);
    ASSERT_EQ(v3v3, 0);
}


// -----------------------------------------------------------------------------
// clear_vertices()
// -----------------------------------------------------------------------------

TEST(LWWGraph, clear_verticesTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);

    data0.clear_vertices(40);
    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.crdt_size(), 3);
}

TEST(LWWGraph, clear_verticesIdempotentTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);

    data0.clear_vertices(40);
    data0.clear_vertices(40);
    data0.clear_vertices(40);
    data0.clear_vertices(40);
    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.crdt_size(), 3);
}

TEST(LWWGraph, clear_verticesWithAddEdgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);
    data0.add_edge("v1", "v1", 20);
    data0.add_edge("v1", "v2", 21);
    data0.add_edge("v2", "v1", 22);

    data0.clear_vertices(30);
    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.crdt_size(), 3);
    auto v1 = data0.crdt_find_vertex("v1");
    auto v2 = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(v1, "v1", true, 30, data0);
    _ASSERT_VERTEX_EQ(v2, "v2", true, 30, data0);

    // Each vertex's edges should be empty
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        auto& edges = it->second.value().edges();
        ASSERT_EQ(edges.size(), 0);
        ASSERT_TRUE(edges.empty());
    }
}

TEST(LWWGraph, clear_verticesReceivedTooLate) {
    LWWGraph<std::string, int, int> data0;

    // Some updates (Done actually before clear)
    data0.add_vertex("v1", 10);
    data0.add_vertex("v2", 10);
    data0.add_vertex("v3", 10);
    data0.add_edge("v1", "v1", 30);
    data0.add_edge("v1", "v2", 30);
    data0.add_edge("v2", "v1", 30);

    // Some updates (Done after clear)
    data0.add_vertex("v4", 50);
    data0.add_vertex("v5", 50);
    data0.add_edge("v1", "v5", 60);
    data0.add_edge("v4", "v2", 60);
    data0.add_edge("v4", "v5", 60);

    // Clear received but newer 'add' exists
    data0.clear_vertices(42);

    // Check vertex v1 and v2 not cleared (Since re-added by add_edge)
    ASSERT_EQ(data0.size(), 4);
    ASSERT_EQ(data0.crdt_size(), 5);
    auto v1_it = data0.crdt_find_vertex("v1");
    auto v2_it = data0.crdt_find_vertex("v2");
    auto v3_it = data0.crdt_find_vertex("v3");
    auto v4_it = data0.crdt_find_vertex("v4");
    auto v5_it = data0.crdt_find_vertex("v5");
    _ASSERT_VERTEX_EQ(v1_it, "v1", false, 60, data0);
    _ASSERT_VERTEX_EQ(v2_it, "v2", false, 60, data0);
    _ASSERT_VERTEX_EQ(v3_it, "v3", true, 42, data0);
    _ASSERT_VERTEX_EQ(v4_it, "v4", false, 60, data0);
    _ASSERT_VERTEX_EQ(v5_it, "v5", false, 60, data0);

    // Check edges status
    auto& v1_edges = v1_it->second.value().edges();
    auto& v2_edges = v2_it->second.value().edges();
    auto& v3_edges = v3_it->second.value().edges();
    auto& v4_edges = v4_it->second.value().edges();
    auto& v5_edges = v5_it->second.value().edges();
    ASSERT_EQ(v1_edges.size(), 1);
    ASSERT_EQ(v2_edges.size(), 0);
    ASSERT_EQ(v3_edges.size(), 0);
    ASSERT_EQ(v4_edges.size(), 2);
    ASSERT_EQ(v5_edges.size(), 0);
}


// -----------------------------------------------------------------------------
// clear_vertex_edges()
// -----------------------------------------------------------------------------

TEST(LWWGraph, clear_vertex_edgesTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);

    data0.add_edge("v1", "v1", 20);
    data0.add_edge("v1", "v2", 20);
    data0.add_edge("v1", "v3", 20);
    data0.add_edge("v2", "v1", 20);
    data0.add_edge("v2", "v3", 20);

    data0.clear_vertex_edges("v1", 40);
    ASSERT_EQ(data0.size(), 3);
    ASSERT_EQ(data0.crdt_size(), 3);

    // Check vertex status
    auto v1_it = data0.crdt_find_vertex("v1");
    auto v2_it = data0.crdt_find_vertex("v2");
    auto v3_it = data0.crdt_find_vertex("v3");
    _ASSERT_VERTEX_EQ(v1_it, "v1", false, 20, data0);
    _ASSERT_VERTEX_EQ(v2_it, "v2", false, 20, data0);
    _ASSERT_VERTEX_EQ(v3_it, "v3", false, 20, data0);

    // Check edges status
    auto& v1_edges = v1_it->second.value().edges();
    auto& v2_edges = v2_it->second.value().edges();
    auto& v3_edges = v3_it->second.value().edges();
    ASSERT_EQ(v1_edges.size(), 0);
    ASSERT_EQ(v2_edges.size(), 2);
    ASSERT_EQ(v3_edges.size(), 0);
}

TEST(LWWGraph, clear_vertex_edgesWithAddCalledLaterTest) {
    LWWGraph<std::string, int, int> data0;

    // Update (Done before clear)
    data0.add_vertex("v1", 10);
    data0.add_vertex("v2", 10);
    data0.add_vertex("v3", 10);

    data0.add_edge("v1", "v1", 20);
    data0.add_edge("v1", "v2", 20);
    data0.add_edge("v2", "v1", 20);
    data0.add_edge("v2", "v3", 20);

    // Update (With timestamp after clear, but applied before clear)
    data0.add_edge("v1", "v3", 40);

    // Clear v1 edges
    data0.clear_vertex_edges("v1", 30);

    ASSERT_EQ(data0.size(), 3);
    ASSERT_EQ(data0.crdt_size(), 3);

    // Check vertex status
    auto v1_it = data0.crdt_find_vertex("v1");
    auto v2_it = data0.crdt_find_vertex("v2");
    auto v3_it = data0.crdt_find_vertex("v3");
    _ASSERT_VERTEX_EQ(v1_it, "v1", false, 40, data0);
    _ASSERT_VERTEX_EQ(v2_it, "v2", false, 20, data0);
    _ASSERT_VERTEX_EQ(v3_it, "v3", false, 40, data0);

    // Check edges status
    auto& v1_edges = v1_it->second.value().edges();
    auto& v2_edges = v2_it->second.value().edges();
    auto& v3_edges = v3_it->second.value().edges();
    ASSERT_EQ(v1_edges.size(), 1);
    ASSERT_EQ(v2_edges.size(), 2);
    ASSERT_EQ(v3_edges.size(), 0);
}

TEST(LWWGraph, clear_vertex_edgesOnInvalidVertexReturnTypeTest) {
    LWWGraph<int, int, int> data0;

    ASSERT_FALSE(data0.clear_vertex_edges(64, 11));
    ASSERT_FALSE(data0.clear_vertex_edges(42, 12));
    ASSERT_FALSE(data0.clear_vertex_edges(32, 13));
}


// -----------------------------------------------------------------------------
// add_vertex()
// -----------------------------------------------------------------------------

TEST(LWWGraph, add_vertexTest) {
    LWWGraph<int, int, int> data0;

    // Add some vertex
    data0.add_vertex(0, 10);
    data0.add_vertex(1, 11);
    data0.add_vertex(2, 12);
    data0.add_vertex(3, 13);
    for(int k = 0; k < 4; ++k) {
        auto res = data0.crdt_find_vertex(k);
        _ASSERT_VERTEX_EQ(res, k, false, (10+k), data0);
    }

    // Add duplicate (With various timestamps but always inferior)
    data0.add_vertex(0, 0);
    data0.add_vertex(1, 9);
    data0.add_vertex(2, 2);
    data0.add_vertex(3, 7);
    for(int k = 0; k < 4; ++k) {
        auto res = data0.crdt_find_vertex(k);
        _ASSERT_VERTEX_EQ(res, k, false, (10+k), data0);
    }

    // Add duplicate, check timestamps is the last value.
    data0.add_vertex(4, 38);
    data0.add_vertex(4, 33);
    data0.add_vertex(4, 39);
    data0.add_vertex(4, 32);
    data0.add_vertex(4, 33);
    auto res = data0.crdt_find_vertex(4);
    _ASSERT_VERTEX_EQ(res, 4, false, 39, data0);
}

TEST(LWWGraph, add_vertexDuplicateCallsTest) {
    LWWGraph<int, int, int> data0;

    // Add duplicate, check timestamps is the last value.
    data0.add_vertex(1, 18);
    data0.add_vertex(1, 13);
    data0.add_vertex(1, 19);
    data0.add_vertex(1, 12);
    data0.add_vertex(1, 13);
    auto res = data0.crdt_find_vertex(1);
    _ASSERT_VERTEX_EQ(res, 1, false, 19, data0);

    // Some more duplicate on another key
    data0.add_vertex(42, 29);
    data0.add_vertex(42, 21);
    data0.add_vertex(42, 23);
    data0.add_vertex(42, 24);
    data0.add_vertex(42, 27);
    res = data0.crdt_find_vertex(42);
    _ASSERT_VERTEX_EQ(res, 42, false, 29, data0);
}

TEST(LWWGraph, add_vertexIdempotentTest) {
    LWWGraph<int, int, int> data0;

    // Add duplicate, check timestamps is the last value.
    data0.add_vertex(1, 10);
    data0.add_vertex(1, 10);
    data0.add_vertex(1, 10);
    ASSERT_EQ(data0.size(), 1);
    ASSERT_EQ(data0.crdt_size(), 1);
    auto res = data0.crdt_find_vertex(1);
    _ASSERT_VERTEX_EQ(res, 1, false, 10, data0);

    // Some more duplicate on another key
    data0.add_vertex(42, 20);
    data0.add_vertex(42, 20);
    data0.add_vertex(42, 20);
    ASSERT_EQ(data0.size(), 2);
    ASSERT_EQ(data0.crdt_size(), 2);
    res = data0.crdt_find_vertex(42);
    _ASSERT_VERTEX_EQ(res, 42, false, 20, data0);
}

TEST(LWWGraph, add_vertexWithOnlyAddReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    // First add vertex
    ASSERT_TRUE(data0.add_vertex("v1", 20));

    // Any other add_vertex call only update timestamp
    ASSERT_FALSE(data0.add_vertex("v1", 10));
    ASSERT_FALSE(data0.add_vertex("v1", 30));
    ASSERT_FALSE(data0.add_vertex("v1", 40));
    ASSERT_FALSE(data0.add_vertex("v1", 15));

    // Add another
    ASSERT_TRUE(data0.add_vertex("v2", 42));
    ASSERT_FALSE(data0.add_vertex("v2", 41));
    ASSERT_FALSE(data0.add_vertex("v2", 43));
}

TEST(LWWGraph, add_vertexWithRemoveVertexReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    ASSERT_TRUE(data0.add_vertex("v1", 20));

    data0.remove_vertex("v1", 10);
    ASSERT_FALSE(data0.add_vertex("v1", 30));

    data0.remove_vertex("v1", 42);
    ASSERT_FALSE(data0.add_vertex("v1", 30));
    ASSERT_TRUE(data0.add_vertex("v1", 64));
    ASSERT_FALSE(data0.add_vertex("v1", 10));
    ASSERT_FALSE(data0.add_vertex("v1", 90));
}


// -----------------------------------------------------------------------------
// remove_vertex()
// -----------------------------------------------------------------------------

TEST(LWWGraph, remove_vertexTest) {
    LWWGraph<std::string, int, int> data0;

    // Add a vertex and remove it
    data0.add_vertex("v1", 11);
    data0.remove_vertex("v1", 12);
    auto res = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 12, data0);

    // Add a vertex with edges
    data0.add_vertex("v1", 20);
    data0.add_vertex("v2", 21);
    data0.add_vertex("v3", 23);
    data0.add_edge("v2", "v3", 24);
    data0.add_edge("v3", "v2", 25);
    data0.add_edge("v2", "v2", 26);
    data0.add_edge("v3", "v3", 27);
    data0.add_edge("v1", "v2", 28);

    // Remove vertex
    data0.remove_vertex("v2", 29);
    res = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(res, "v2", true, 29, data0);

    // All edges with v2 (to and from) should be removed.
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        auto& edges = it->second.value().edges();
        auto edge_it = edges.crdt_find("v2");
        bool isRemoved = edge_it->second.isRemoved();
        int timestamp = edge_it->second.timestamp();

        ASSERT_TRUE(edge_it != edges.crdt_end()); // They all add edge with v2
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 29);
    }
}

TEST(LWWGraph, remove_vertexBeforeVertexCreatedTest) {
    LWWGraph<std::string, int, int> data0;

    // Remove vertex before even added (Remove it anyway)
    data0.remove_vertex("v1", 20);
    auto res = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 20, data0);

    // Receive the add, but too late (Vertex is still the same)
    data0.add_vertex("v1", 10);
    res = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 20, data0);
}

TEST(LWWGraph, remove_vertexDuplicateCallsTest) {
    LWWGraph<std::string, int, int> data0;

    // Add, then remove vertex
    data0.add_vertex("v1", 10);
    data0.remove_vertex("v1", 11);
    auto res = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 11, data0);

    // Duplicate remove calls
    data0.remove_vertex("v1", 22);
    data0.remove_vertex("v1", 28);
    data0.remove_vertex("v1", 29);
    data0.remove_vertex("v1", 21);
    data0.remove_vertex("v1", 27);
    data0.remove_vertex("v1", 24);
    res = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 29, data0);
}

TEST(LWWGraph, remove_vertexIdempotentTest) {
    LWWGraph<std::string, int, int> data0;

    // Add, then remove vertex
    data0.add_vertex("v1", 10);
    data0.remove_vertex("v1", 20);
    data0.remove_vertex("v1", 20);
    data0.remove_vertex("v1", 20);

    // Check success
    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.crdt_size(), 1);
    auto res = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 20, data0);

    // Add another, remove and test
    data0.add_vertex("v2", 30);
    data0.remove_vertex("v2", 40);
    data0.remove_vertex("v2", 40);
    data0.remove_vertex("v2", 40);

    // Check success
    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.crdt_size(), 2);
    res = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(res, "v2", true, 40, data0);
}

TEST(LWWGraph, remove_vertexReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 20);
    ASSERT_FALSE(data0.remove_vertex("v1", 10));
    ASSERT_TRUE(data0.remove_vertex("v1", 30));
    ASSERT_FALSE(data0.remove_vertex("v1", 29));
}

TEST(LWWGraph, remove_vertexWithRemoveCalledBeforeAddReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    ASSERT_FALSE(data0.remove_vertex("v1", 20));

    // Add vertex, but already remove later so still return false
    data0.add_vertex("v1", 15);
    ASSERT_FALSE(data0.remove_vertex("v1", 10));
    ASSERT_FALSE(data0.remove_vertex("v1", 30));
    ASSERT_FALSE(data0.remove_vertex("v1", 40));
}

TEST(LWWGraph, remove_vertexWithAddVertexReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 20);
    ASSERT_FALSE(data0.remove_vertex("v1", 10));
    ASSERT_TRUE(data0.remove_vertex("v1", 30));
    ASSERT_FALSE(data0.remove_vertex("v1", 29));
    ASSERT_FALSE(data0.remove_vertex("v1", 31));
}


// -----------------------------------------------------------------------------
// add_edge()
// -----------------------------------------------------------------------------

TEST(LWWGraph, add_edgeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_edge("v1", "v2", 13);

    // Vertex 1 is 're-added' by add_edge (So timestamps == add_edge timestamps)
    auto v1 = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 13, data0);

    // Vertex 2 is 're-added' by add_edge (So timestamps == add_edge timestamps)
    auto v2 = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", false, 13, data0);

    // Edge v1 -> v2 should have been created
    auto edges = v1->second.value().edges();
    auto edge = edges.crdt_find("v2");
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 13);
    EXPECT_FALSE(edge->second.isRemoved());
}

TEST(LWWGraph, add_edgeBeforeVertexCreatedTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_edge("v1", "v2", 10);

    // Vertex 1 should have been created
    auto v1 = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 10, data0);

    // Vertex 2 should have been created
    auto v2 = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", false, 10, data0);

    // Edge v1 -> v2 should have been created and marked as removed
    auto edges = v1->second.value().edges();
    auto edge = edges.crdt_find("v2"); // LWWSet. Returns const_crdt_iterator
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 10);
    EXPECT_FALSE(edge->second.isRemoved());


    // Now receive the actual 'add_vertex'. (But add_edge created earlier stamp)
    data0.add_vertex("v1", 1);
    data0.add_vertex("v2", 2);

    v1 = data0.crdt_find_vertex("v1");
    v2 = data0.crdt_find_vertex("v2");
    edges = v1->second.value().edges();
    edge = edges.crdt_find("v2");
    EXPECT_EQ(v1->second.timestamp(), 10);
    EXPECT_EQ(v2->second.timestamp(), 10);
    EXPECT_EQ(edge->second.timestamp(), 10);
}

TEST(LWWGraph, add_edgeDuplicateCallsTest) {
    LWWGraph<std::string, int, int> data0;

    // Received many duplicate add_edge calls
    data0.add_edge("v1", "v2", 10);
    data0.add_edge("v1", "v2", 17);
    data0.add_edge("v1", "v2", 19); // Winner
    data0.add_edge("v1", "v2", 13);
    data0.add_edge("v1", "v2", 11);
    data0.add_edge("v1", "v2", 12);
    data0.add_edge("v1", "v2", 18);

    // Vertex 1
    auto v1 = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 19, data0);

    // Vertex 2
    auto v2 = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", false, 19, data0);

    // Edge v1 -> v2
    auto edges = v1->second.value().edges();
    auto edge = edges.crdt_find("v2");
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 19);
    EXPECT_FALSE(edge->second.isRemoved());
}

TEST(LWWGraph, add_edgeIdempotentTest) {
    LWWGraph<std::string, int, int> data0;

    // Duplicate add_edge
    data0.add_edge("v1", "v2", 10);
    data0.add_edge("v1", "v2", 10);
    data0.add_edge("v1", "v2", 10);

    ASSERT_EQ(data0.size(), 2);
    ASSERT_EQ(data0.crdt_size(), 2);

    // Vertex 1 and 2
    auto v1 = data0.crdt_find_vertex("v1");
    auto v2 = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 10, data0);
    _ASSERT_VERTEX_EQ(v2, "v2", false, 10, data0);

    // Edge v1 -> v2
    auto edges = v1->second.value().edges();
    auto edge = edges.crdt_find("v2");
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 10);
    EXPECT_FALSE(edge->second.isRemoved());
}

TEST(LWWGraph, add_edgeWithFromToEqualTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 11);
    data0.add_edge("v1", "v1", 21);
    data0.add_edge("v1", "v1", 29);
    data0.add_edge("v1", "v1", 13);

    auto v1_it = data0.crdt_find_vertex("v1");
    auto v1_edge = v1_it->second.value().edges().crdt_find("v1");
    EXPECT_TRUE(v1_edge != v1_it->second.value().edges().crdt_end());
    EXPECT_EQ(v1_edge->first, "v1");
    EXPECT_EQ(v1_edge->second.timestamp(), 29);
    EXPECT_FALSE(v1_edge->second.isRemoved());
}

TEST(LWWGraph, add_edgeReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 11);

    auto res1 = data0.add_edge("v1", "v1", 20);
    auto res2 = data0.add_edge("v1", "v1", 19);
    auto res3 = data0.add_edge("v1", "v1", 21);

    _ASSERT_ADD_EDGE_INFO_EQ(res1, true, false, false);
    _ASSERT_ADD_EDGE_INFO_EQ(res2, false, false, false);
    _ASSERT_ADD_EDGE_INFO_EQ(res3, false, false, false);
}

TEST(LWWGraph, add_edgeBeforeAddVertexReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    auto coco = data0.add_edge("v1", "v2", 42);
    _ASSERT_ADD_EDGE_INFO_EQ(coco, true, true, true);

    // Since add_edge internally created v1 and v2, add_vertex for v1 and v2
    // does nothing (From end user point of view. Internally update timestamps).
    ASSERT_FALSE(data0.add_vertex("v1", 10));
    ASSERT_FALSE(data0.add_vertex("v2", 20));
}

TEST(LWWGraph, add_edgeWithToAndFromEqualReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    // v1 (Edge without add vertex)
    auto coco = data0.add_edge("v1", "v1", 42);
    _ASSERT_ADD_EDGE_INFO_EQ(coco, true, true, false); // See doc

    // v2 (Edge with add vertex first)
    data0.add_vertex("v2", 11);
    coco = data0.add_edge("v2", "v2", 20);
    _ASSERT_ADD_EDGE_INFO_EQ(coco, true, false, false);
}


// -----------------------------------------------------------------------------
// remove_edge()
// -----------------------------------------------------------------------------

TEST(LWWGraph, remove_edgeTest) {
    LWWGraph<std::string, int, int> data0;

    // Add vertex + edge
    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.remove_edge("v1", "v2", 13);

    // Vertex 1 stay the same
    auto v1 = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 11, data0);

    // Vertex 2 stay the same
    auto v2 = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", false, 12, data0);

    // Edge v1 -> v2 should have been removed
    auto edges = v1->second.value().edges();
    auto edge = edges.crdt_find("v2");
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 13);
    EXPECT_TRUE(edge->second.isRemoved());
}

TEST(LWWGraph, remove_edgeIdempotentTest) {
    LWWGraph<std::string, int, int> data0;

    // Add vertex + edge
    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.remove_edge("v1", "v2", 13);
    data0.remove_edge("v1", "v2", 13);
    data0.remove_edge("v1", "v2", 13);
    data0.remove_edge("v1", "v2", 13);

    // Vertex 1 stay the same
    auto v1 = data0.crdt_find_vertex("v1");
    auto v2 = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(v1, "v1", false, 11, data0);
    _ASSERT_VERTEX_EQ(v2, "v2", false, 12, data0);

    // Edge v1 -> v2 should have been removed
    auto edges = v1->second.value().edges();
    auto edge = edges.crdt_find("v2");
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 13);
    EXPECT_TRUE(edge->second.isRemoved());
}

TEST(LWWGraph, remove_edgeBeforeAddedTest) {
    LWWGraph<std::string, int, int> data0;

    // Remove edge before added. Two tmp vertex are added.
    data0.remove_edge("v1", "v2", 10);

    // vertex 1 should have been created (with tmp timestamps)
    auto v1 = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(v1, "v1", true, 0, data0); // Smallest timestamp.

    // vertex 2 should have been created (with tmp timestamps)
    auto v2 = data0.crdt_find_vertex("v2");
    _ASSERT_VERTEX_EQ(v2, "v2", true, 0, data0); // Smallest timestamp.

    // Edge v1 -> v2 should have been created and marked as removed
    auto edges = v1->second.value().edges();
    auto edge = edges.crdt_find("v2"); // LWWSet. Returns const_crdt_iterator
    EXPECT_TRUE(edge != edges.crdt_end());
    EXPECT_EQ(edge->first, "v2");
    EXPECT_EQ(edge->second.timestamp(), 10);
    EXPECT_TRUE(edge->second.isRemoved());


    // Duplicate remove calls
    // All timestamps stay same (v1, v2, edge)
    data0.remove_edge("v1", "v2", 2);
    data0.remove_edge("v1", "v2", 1);
    data0.remove_edge("v1", "v2", 3);
    v1 = data0.crdt_find_vertex("v1");
    v2 = data0.crdt_find_vertex("v2");
    edges = v1->second.value().edges();
    edge = edges.crdt_find("v2");
    EXPECT_EQ(v1->second.timestamp(), 0);
    EXPECT_EQ(v2->second.timestamp(), 0);
    EXPECT_EQ(edge->second.timestamp(), 10);


    // Duplicate remove calls with higher timestamps
    // Vertex still have same timestamps. Edge is updated.
    data0.remove_edge("v1", "v2", 24);
    data0.remove_edge("v1", "v2", 20);
    data0.remove_edge("v1", "v2", 29);
    data0.remove_edge("v1", "v2", 28);
    v1 = data0.crdt_find_vertex("v1");
    v2 = data0.crdt_find_vertex("v2");
    edges = v1->second.value().edges();
    edge = edges.crdt_find("v2");
    EXPECT_EQ(v1->second.timestamp(), 0);
    EXPECT_EQ(v2->second.timestamp(), 0);
    EXPECT_EQ(edge->second.timestamp(), 29);
}

TEST(LWWGraph, remove_edgeReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    data0.add_vertex("v1", 10);
    data0.add_vertex("v1", 20);
    data0.add_edge("v1", "v2", 30);

    ASSERT_TRUE(data0.remove_edge("v1", "v2", 40));
}

TEST(LWWGraph, remove_edgeCalledBeforeAddEdgeReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    ASSERT_FALSE(data0.remove_edge("v1", "v2", 20));

    // Edge already removed. But add vertex anyway
    auto coco1 = data0.add_edge("v1", "v2", 10);
    auto coco2 = data0.add_edge("v1", "v2", 19);

    _ASSERT_ADD_EDGE_INFO_EQ(coco1, false, true, true);
    _ASSERT_ADD_EDGE_INFO_EQ(coco2, false, false, false);


    // Add after remove (Re-add)
    auto coco3 = data0.add_edge("v1", "v2", 30);
    auto coco4 = data0.add_edge("v1", "v2", 31);

    _ASSERT_ADD_EDGE_INFO_EQ(coco3, true, false, false);
    _ASSERT_ADD_EDGE_INFO_EQ(coco4, false, false, false);
}

TEST(LWWGraph, remove_edgeCalledBeforeAddVertexReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    // Mark edge as removed + creates tmp vertex with minimal timestamp.
    ASSERT_FALSE(data0.remove_edge("v1", "v2", 20));

    // Add the vertex creates them for user view (But only updates time)
    ASSERT_TRUE(data0.add_vertex("v1", 10));
    ASSERT_TRUE(data0.add_vertex("v2", 20));
}

TEST(LWWGraph, remove_edgeFirstThenAddEdgeThenAddVertexReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;

    // Mark edge as removed + creates tmp vertex with minimal timestamp.
    ASSERT_FALSE(data0.remove_edge("v1", "v2", 42));

    // Add edge, but already removed. User doesn't see anything
    auto coco1 = data0.add_edge("v1", "v2", 20);
    auto coco2 = data0.add_edge("v1", "v2", 30); // Duplicate call

    _ASSERT_ADD_EDGE_INFO_EQ(coco1, false, true, true);
    _ASSERT_ADD_EDGE_INFO_EQ(coco2, false, false, false);

    // Add the vertex but already created by add_edge
    ASSERT_FALSE(data0.add_vertex("v1", 10));
    ASSERT_FALSE(data0.add_vertex("v2", 20));
}


// -----------------------------------------------------------------------------
// add_edge() || remove_vertex()
// -----------------------------------------------------------------------------

TEST(LWWGraph, remove_vertexWithEdgesDuplicateCallsTest) {
    LWWGraph<std::string, int, int> data0;

    // Create graph
    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);
    data0.add_vertex("v4", 14);

    data0.add_edge("v1", "v1", 21);
    data0.add_edge("v1", "v2", 22);
    data0.add_edge("v1", "v3", 23);
    data0.add_edge("v1", "v4", 24);

    data0.add_edge("v2", "v1", 25);
    data0.add_edge("v2", "v3", 26);

    data0.add_edge("v3", "v4", 27);
    data0.add_edge("v3", "v1", 28);

    data0.add_edge("v4", "v1", 29);

    data0.remove_vertex("v1", 30);

    // Remove vertex, should also remove all edges
    auto res = data0.crdt_find_vertex("v1");
    _ASSERT_VERTEX_EQ(res, "v1", true, 30, data0);
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        auto& edges = it->second.value().edges();
        auto edge_it = edges.crdt_find("v1");
        bool isRemoved = edge_it->second.isRemoved();
        int timestamp = edge_it->second.timestamp();

        ASSERT_TRUE(edge_it != edges.crdt_end()); // They all add edge with v1
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 30);
    }
}

TEST(LWWGraph, add_edgeRemoveVertexConcurrentTest) {
    LWWGraph<std::string, int, int> data0;

    // Someone create a vertex and delete it.
    data0.add_vertex("v1", 10);
    data0.remove_vertex("v1", 100);

    // Someone added the same vertex with edge (But earlier in time).
    data0.add_vertex("v1", 10);
    data0.add_vertex("v2", 11);
    data0.add_vertex("v3", 13);
    data0.add_edge("v1", "v2", 21);
    data0.add_edge("v1", "v3", 22);
    data0.add_edge("v2", "v1", 23);
    data0.add_edge("v1", "v1", 24);
    data0.add_edge("v3", "v1", 25);

    // Remove timestamp is last, add_edge must remove edges linked with v1
    // Even if add_edge is called after remove (ex: network latency)

    // V1 is deleted as well as all its edges
    auto v1_it = data0.crdt_find_vertex("v1");
    EXPECT_TRUE(v1_it != data0.crdt_end());
    EXPECT_EQ(v1_it->first, "v1");
    EXPECT_EQ(v1_it->second.timestamp(), 100);
    EXPECT_TRUE(v1_it->second.isRemoved());
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        auto& edges = it->second.value().edges();
        auto edge_it = edges.crdt_find("v1");
        bool isRemoved = edge_it->second.isRemoved();
        int timestamp = edge_it->second.timestamp();

        ASSERT_TRUE(edge_it != edges.crdt_end()); // They all add edge with v1
        ASSERT_TRUE(isRemoved);
        ASSERT_EQ(timestamp, 100);
    }
}

TEST(LWWGraph, remove_vertexAddEdgeWithVertexReaddedTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // data0 initial state
    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);
    data0.add_edge("v1", "v2", 21);
    data0.add_edge("v2", "v3", 22);

    // data1 initial state
    data1.add_vertex("v1", 11);
    data1.add_vertex("v2", 12);
    data1.add_vertex("v3", 13);
    data1.add_edge("v1", "v2", 21);
    data1.add_edge("v2", "v3", 22);

    // For now, data0 == data1
    ASSERT_TRUE(data0 == data1);
    ASSERT_TRUE(data0.crdt_equal(data1));


    // data1 remove vertex
    data1.remove_vertex("v2", 30);
    // data0 add edge v2 -> v2, which re-add the v2 vertex.
    data0.add_edge("v2", "v2", 40);

    // Broadcast changes
    data0.remove_vertex("v2", 30);
    data1.add_edge("v2", "v2", 40);

    // Should be same
    ASSERT_TRUE(data0 == data1);
    ASSERT_TRUE(data0.crdt_equal(data1));

    // Check internal state of v1, v2 and v3
    _ASSERT_VERTEX_EQ(data0.crdt_find_vertex("v1"), "v1", false, 21, data0);
    _ASSERT_VERTEX_EQ(data0.crdt_find_vertex("v2"), "v2", false, 40, data0);
    _ASSERT_VERTEX_EQ(data0.crdt_find_vertex("v3"), "v3", false, 22, data0);
}

TEST(LWWGraph, remove_vertexAddEdgeWithVertexReaddedReturnTypeTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // data0 initial state
    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);
    data0.add_edge("v1", "v2", 21);
    data0.add_edge("v2", "v3", 22);

    // data1 initial state
    data1.add_vertex("v1", 11);
    data1.add_vertex("v2", 12);
    data1.add_vertex("v3", 13);
    data1.add_edge("v1", "v2", 21);
    data1.add_edge("v2", "v3", 22);

    // For now, data0 == data1
    ASSERT_TRUE(data0 == data1);
    ASSERT_TRUE(data0.crdt_equal(data1));


    // data1 remove vertex. data0 add edge v2->v2 (Re-add v2)
    ASSERT_TRUE(data1.remove_vertex("v2", 30));
    auto coco0 = data0.add_edge("v2", "v2", 40);
    _ASSERT_ADD_EDGE_INFO_EQ(coco0, true, false, false);

    // Broadcast changes
    ASSERT_FALSE(data0.remove_vertex("v2", 30)); // Not actually applied: false
    auto coco1 = data1.add_edge("v2", "v2", 40);
    _ASSERT_ADD_EDGE_INFO_EQ(coco1, true, true, false);

    // Should be same
    ASSERT_TRUE(data0 == data1);
    ASSERT_TRUE(data0.crdt_equal(data1));

    // Check internal state of v1, v2 and v3
    _ASSERT_VERTEX_EQ(data0.crdt_find_vertex("v1"), "v1", false, 21, data0);
    _ASSERT_VERTEX_EQ(data0.crdt_find_vertex("v2"), "v2", false, 40, data0);
    _ASSERT_VERTEX_EQ(data0.crdt_find_vertex("v3"), "v3", false, 22, data0);
}


// -----------------------------------------------------------------------------
// crdt_size()
// -----------------------------------------------------------------------------

TEST(LWWGraph, crdt_sizeWithadd_edgeremove_vertex) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // This illustrate the scenario of concurrent add_edge || remove_vertex

    // init data0
    data0.add_vertex("v1", 110);
    data0.add_vertex("v2", 120);
    data0.add_edge("v1", "v2", 130);

    // init data1
    data1.add_vertex("v1", 110);
    data1.add_vertex("v2", 120);
    data1.add_edge("v1", "v2", 130);

    // data0 apply add_edge first
    data0.add_edge("v2", "v3", 140);
    data0.remove_vertex("v1", 150);

    // data1 apply remove_vertex first
    data1.remove_vertex("v1", 150);
    data1.add_edge("v2", "v3", 140);

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
    data0.add_vertex("v1", 11);
    data0.add_vertex("v2", 12);
    data0.add_vertex("v3", 13);
    data0.add_vertex("v4", 14);
    data0.add_vertex("v5", 15);

    // Replicate data1
    data1.add_vertex("v6", 21);
    data1.add_vertex("v7", 22);
    data1.add_vertex("v8", 23);
    data1.add_vertex("v9", 24);

    // At this point, data0 != data1
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast data0 to data1
    data1.add_vertex("v1", 11);
    data1.add_vertex("v2", 12);
    data1.add_vertex("v3", 13);
    data1.add_vertex("v4", 14);
    data1.add_vertex("v5", 15);

    // Broadcast data1 to data0
    data0.add_vertex("v6", 21);
    data0.add_vertex("v7", 22);
    data0.add_vertex("v8", 23);
    data0.add_vertex("v9", 24);

    // Now, data0 == data1
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));
}

TEST(LWWGraph, crdt_equalWithAddVertexRemoveVertexTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // Replicate data0
    data0.add_vertex("v1", 110);
    data0.add_vertex("v2", 120);
    data0.add_vertex("v3", 130);
    data0.remove_vertex("v1", 140);
    data0.remove_vertex("v3", 150);
    data0.add_vertex("v1", 150);

    // Replicate data1
    data1.add_vertex("v5", 101);
    data1.remove_vertex("v5", 111);
    data1.add_vertex("v2", 111);
    data1.add_vertex("v1", 111);
    data1.add_vertex("v5", 121);
    data1.add_vertex("v6", 131);
    data1.remove_vertex("v1", 201);

    // Atm, data0 != data1
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));


    // Broadcast data0 to data1
    data1.add_vertex("v1", 110);
    data1.add_vertex("v2", 120);
    data1.add_vertex("v3", 130);
    data1.remove_vertex("v1", 140);
    data1.remove_vertex("v3", 150);
    data1.add_vertex("v1", 150);

    // Broadcast data1 to data0
    data0.add_vertex("v5", 101);
    data0.remove_vertex("v5", 111);
    data0.add_vertex("v2", 111);
    data0.add_vertex("v1", 111);
    data0.add_vertex("v5", 121);
    data0.add_vertex("v6", 131);
    data0.remove_vertex("v1", 201);

    // Now, data0 == data1
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));
}

TEST(LWWGraph, crdt_equalWithAddEdgeRemoveVertexTest) {
    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // This illustrate the scenario of concurrent add_edge || remove_vertex

    // Replicate data0
    data0.add_vertex("v1", 110);
    data0.add_vertex("v2", 120);
    data0.add_edge("v1", "v2", 130);
    data0.add_edge("v1", "v3", 200);
    data0.add_edge("v2", "v1", 210);

    // Replicate data1
    data1.add_vertex("v1", 111);
    data1.add_vertex("v2", 121);
    data1.add_edge("v1", "v2", 131);
    data1.remove_vertex("v1", 301);


    // data0 != data1
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast data0 to data1
    data1.add_vertex("v1", 110);
    data1.add_vertex("v2", 120);
    data1.add_edge("v1", "v2", 130);
    data1.add_edge("v1", "v3", 200);
    data1.add_edge("v2", "v1", 210);

    // Broadcast data1 to data0
    data0.add_vertex("v1", 111);
    data0.add_vertex("v2", 121);
    data0.add_edge("v1", "v2", 131);
    data0.remove_vertex("v1", 301);


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
    data0.add_vertex("v1", 10);
    data0.add_vertex("v2", 10);
    data0.add_vertex("v3", 10);

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
    data0.add_vertex("v1", 1);
    data0.add_vertex("v2", 2);
    data0.add_vertex("v3", 3);
    data0.add_vertex("v4", 4);
    data0.add_vertex("v5", 5);
    data0.add_vertex("v6", 6);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);

    data0.add_edge("v1", "v3", 7);
    data0.add_edge("v2", "v1", 8);
    data0.add_edge("v2", "v4", 9);
    data0.add_edge("v3", "v2", 10);
    data0.add_edge("v4", "v5", 11);
    data0.add_edge("v6", "v3", 12);
    data0.add_edge("v6", "v4", 13);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);

    data0.remove_edge("v6", "v3", 14);
    data0.remove_edge("v6", "v4", 15);
    data0.remove_edge("v3", "v2", 16);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);


    // Replicate 1
    data1.add_vertex("v1", 1);
    data1.add_vertex("v2", 2);
    data1.add_vertex("v3", 3);
    data1.add_vertex("v4", 4);
    data1.add_vertex("v5", 5);
    data1.add_vertex("v6", 6);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);

    data1.add_edge("v1", "v3", 7);
    data1.add_edge("v2", "v1", 8);
    data1.add_edge("v2", "v4", 9);
    data1.add_edge("v3", "v2", 10);
    data1.add_edge("v4", "v5", 11);
    data1.add_edge("v6", "v3", 12);
    data1.add_edge("v6", "v4", 13);

    EXPECT_FALSE(data0 == data1);
    EXPECT_TRUE(data0 != data1);

    data1.remove_edge("v6", "v3", 14);
    data1.remove_edge("v6", "v4", 15);
    data1.remove_edge("v3", "v2", 16);

    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);
}


}} // End namespaces


