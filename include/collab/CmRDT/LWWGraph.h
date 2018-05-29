#pragma once

#include "LWWMap.h"
#include "LWWSet.h"

#include <ostream>
#include <cassert>
#include <type_traits>

namespace collab {
namespace CmRDT {


/**
 * \brief
 * Last-Writer-Wins Directed Graph (LWW Graph).
 * CmRDT (Operation-based)
 *
 * This is a Directed Graph. Internally uses adjacency list of vertex (Using
 * LWWMap). Each vertex has a set of edge destination (Using LWWSet).
 * Vertex content is user defined (See warning below).
 *
 * \par CRDT properties
 * Timestamps is assigned to each add / remove operation to create total order
 * of operations. Whenever add or remove operation is applied, timestamps are
 * used to select the winning operation.
 *
 * \par
 * Any added vertex is never removed but only marked as deleted instead.
 * For that reason, this CRDT container may not fit all system due to memory use.
 *
 * \par
 * All operations on this container are commutative! You may receive a remove
 * operation even before its create operation (ex: Network latency).
 * To be fully CRDT without any synchronization mechanism, order of operations
 * is not important.
 * In several examples and implementations, a 'pre condition' is
 * required in order to apply some operations. Whenever this pre condition is
 * not validated, external synchronization mechanism is required. (Operation
 * is delayed until possible). An example of implementation is to use
 * dependency metadata. This may be important, for instance in order to have
 * causality preservation. I decided to design all operation to be commutative
 * so that no synchronization, delay or external mechanism are required.
 *
 * \par Quote from the article "A comprehensive study of CRDT"
 * "
 *  A Last-Writer-Wins Register (LWW-Register) creates a total order of
 *  assignments by associating a timestamp with each update.
 *  Timestamps are assumed unique, totally ordered, and consistent with causal
 *  order; i.e., if assignment 1 happened-before assignment 2,
 *  the former's timestamp is less than the latter's. This may be implemented as
 *  a per-replicate counter concatenated with a unique replica identifier,
 *  such as its MAC address.
 * "
 *
 * \warning
 * CRDT Graph only deals with concurrent add / remove of vertex and edges!!
 * By default, vertex's content is NOT CRDT! This means, several user working
 * concurrently eventually end up with the same graph but may have
 * different vertex data. This is because we cannot deal with add / remove
 * and vertex content update at the same place. For instance, you may have a
 * 'graph of map of register' and this container don't know how to update its
 * content.
 * To have a graph of CRDT atomic content, you may use a graph of LWWRegister
 * for instance. To update a vertex data, use query on this vertex and call the
 * register update function.
 *
 * \warning
 * Timestamps are strictly unique with total order.
 * If (t1 == t2) is true, replicates may diverge.
 * (See quote and implementation for further informations).
 *
 * \warning
 * T template parameter must have a default constructor.
 * U timestamp must accept "U t = 0" (This should set with minimal value).
 *
 *
 * \tparam Key  Type of unique identifier for each graph vertex
 * \tparam T    Type of vertex content data.
 * \tparam U    Type of timestamps (Must implements operators > and <).
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename T, typename U>
class LWWGraph {
    private:
        class Vertex;
        LWWMap<Key, Vertex, U> _adj;

        typedef typename LWWMap<Key,Vertex,U>::crdt_iterator crdt_iterator;


    // -------------------------------------------------------------------------
    // CRDT methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Query a vertex and its internal CRDT metadata.
         *
         * If vertex with this key exists in the internal container, it is
         * returned, regardless its 'removed' status.
         * This may be useful for datatypes built on top of this set.
         * To have CRDT updates, you must apply the update, regardless its
         * internal status. This query will return the vertex in any case
         * (If already added once in the container.)
         *
         * If this key has never been added, returns crdt past-the-end
         * (See crdt_end()) iterator.
         *
         * \param key   Vertex's key to find.
         * \return      Iterator to the vertex or crdt_end() if not found.
         */
        crdt_iterator queryVertex(const Key& key) {
            return _adj.query(key);
        }

        /**
         * Add a new vertex in the graph.
         *
         * If key already exists, use timestamps for concurrency control.
         *
         * \par Concurrent addVertex / addVertex
         * Timestamp is updated with the higher value. Key is added in any case.
         *
         * \par Concurrent addVertex / removeVertex
         * Uses the higher timestamp select the winning operation.
         * If remove timestamp wins, this add operation does nothing.
         *
         * \note
         * This only adds the key. A default vertex is created.
         * To add key and set its content, call this add method and query the
         * newly added vertex (Then update it with its default value).
         *
         * \param key   The unique vertex's key.
         * \param stamp Timestamp of this operation.
         */
        void addVertex(const Key& key, const U& stamp) {
            _adj.add(key, stamp);
        }

        /**
         * Remove a vertex from the graph.
         *
         * If key doesn't exists, internally add it first (with removed flag).
         * This is because remove / add are commutative and remove may be
         * received before add. (Note that receiving the actual add wont do
         * anything since its timestamps will be smaller).
         *
         * After timestamp check, if vertex actually removed, also remove all
         * edges that implies this vertex.
         *
         * \par Concurrent addEdge / removeVertex
         * See documentation of LWWGraph::addEdge
         *
         * \see LWWGraph::addEdge
         *
         * \param key   The unique vertex's key.
         * \param stamp Timestamp of this operation.
         */
        void removeVertex(const Key& key, const U& stamp) {
            _adj.remove(key, stamp);

            // Remove all edge to this vertex (Even from 'removed' vertex)
            for(auto it = _adj.crdt_begin(); it != _adj.crdt_end(); ++it) {
                auto& edges = it->second.value()._edges;
                edges.remove(key, stamp);
            }
        }

        /**
         * Add edge from a vertex to another.
         *
         * This also performs a 'addVertex' for vertex 'from' and 'to'.
         * This ensure addEdge is commutative even if addEdge is received before
         * the add operations.
         *
         * \par Concurrent addEdge / removeVertex
         * If after the 'addVertex' operation, any vertex is still marked as
         * removed. Meaning 'addEdge' was before 'removeVertex', this edge
         * is marked as removed (With the 'removeVertex' timestamp).
         * This resolve the concurrent 'addEdge' | 'removeVertex'
         *
         * \param from  The origin vertex.
         * \param to    The destination vertex.
         */
        void addEdge(const Key& from, const Key& to, const U& stamp) {
            _adj.add(from, stamp);
            _adj.add(to, stamp);

            auto res = _adj.query(from);
            Vertex &v = res->second.value();
            v._edges.add(to, stamp);

            // If on vertex is removed, also remove this edge with highest U.
            // This is really important for CRDT / Commutativity feature.
            auto vertex_it_from = _adj.query(from);
            auto vertex_it_to = _adj.query(to);
            assert(vertex_it_from != _adj.crdt_end());
            assert(vertex_it_to != _adj.crdt_end());
            const bool from_removed = vertex_it_from->second.isRemoved();
            const bool to_removed = vertex_it_to->second.isRemoved();

            if(from_removed || to_removed) {
                U from_time = vertex_it_from->second.timestamp();
                U to_time   = vertex_it_to->second.timestamp();
                assert(to_time != from_time);
                U new_time  = (from_time > to_time) ? from_time : to_time;

                auto res = _adj.query(from);
                Vertex &v = res->second.value();
                v._edges.remove(to, new_time);
            }
        }

        /**
         * Removes the specific edge between two vertex.
         *
         * If from and/or to doesn't exists, create one with minimum possible
         * timestamp. This create a temporary vertex. This is use in case of
         * removeEdge is applied before 'addEdge'. (Required for CRDT
         * commutativity).
         *
         * \param from  The origin vertex.
         * \param to    The destination vertex.
         */
        void removeEdge(const Key& from, const Key& to, const U& stamp) {
            _adj.remove(from, 0);
            _adj.remove(to, 0);

            auto res = _adj.query(from);
            Vertex &v = res->second.value();
            v._edges.remove(to, stamp);
        }


    // -------------------------------------------------------------------------
    // Iterator
    // -------------------------------------------------------------------------

    public:

        /**
         * Returns a constant crdt iterator to the beginning.
         *
         * \see crdt_iterator
         * \return CRDT iterator to the first vertex.
         */
        crdt_iterator crdt_begin() {
            return _adj.crdt_begin();
        }

        /**
         * Returns a constant crdt iterator to the end.
         *
         * \see crdt_iterator
         * \return CRDT iterator to the last vertex.
         */
        crdt_iterator crdt_end() {
            return _adj.crdt_end();
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if lhs and rhs are equals.
         * Two LWWGraphs are equal if their adjacency list of 'living' vertex
         * are equal.
         *
         * \param lhs Left hand side
         * \param rhs Right hand side
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const LWWGraph& lhs, const LWWGraph& rhs) {
            if(lhs._adj.size() != rhs._adj.size()) {
                return false;
            }
            return (lhs._adj == rhs._adj);
        }

        /**
         * Check if lhs and rhs are not equals.
         * See operator == for further information about equality meaning.
         *
         * \see LWWGraph::operator==
         *
         * \param lhs Left hand side
         * \param rhs Right hand side
         * \return True if not equal, otherwise, return false.
         */
        friend bool operator!=(const LWWGraph& lhs, const LWWGraph& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Display the internal graph content.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out,
                                        const LWWGraph<Key,T,U>& o) {
            out << "CmRDT::LWWGraph = Not Implemented Yet";
            // TODO To implements
            return out;
        }
};


// /////////////////////////////////////////////////////////////////////////////
// *****************************************************************************
// Nested classes
// *****************************************************************************
// /////////////////////////////////////////////////////////////////////////////

/**
 * \brief
 * Vertex data int the graph.
 *
 *
 * \tparam Key  Type of key.
 * \tparam T    Type of element.
 * \tparam U    Type of timestamps.
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename T, typename U>
class LWWGraph<Key,T,U>::Vertex {

    private:
        friend LWWGraph;
        T               _content;
        LWWSet<Key,U>   _edges;

    public:

        /**
         * Returns a reference to the vertex content data.
         *
         * \return Reference to the content.
         */
        T& content() {
            return _content;
        }

        /**
         * Returns a reference to the vertex's edges.
         *
         * \return Reference to the set of edges.
         */
        LWWSet<Key,U>& edges() {
            return _edges;
        }

        /**
         * Check if lhs and rhs are equals.
         * Two Vertex are equal if their set of edges are equal and their
         * content are equal.
         *
         * \param lhs Left hand side
         * \param rhs Right hand side
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const Vertex& lhs, const Vertex& rhs) {
            return (lhs._edges == rhs._edges) && (lhs._content == rhs._content);
        }

        /**
         * Check if lhs and rhs are not equals.
         * See operator == for further information about equality meaning.
         *
         * \see LWWMap::Vertex::operator==
         *
         * \param lhs Left hand side
         * \param rhs Right hand side
         * \return True if not equal, otherwise, return false.
         */
        friend bool operator!=(const Vertex& lhs, const Vertex& rhs) {
            return !(lhs == rhs);
        }
};


}} // End namespaces


