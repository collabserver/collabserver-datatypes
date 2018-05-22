#pragma once

#include "LWWMap.h"
#include "LWWSet.h"

#include <ostream>

namespace CRDT {
namespace CmRDT {


/**
 * Last-Writer-Wins Graph (LWW Graph).
 * CmRDT (Operation-based)
 *
 * This is a Directed Graph.
 *
 * \note
 * CRDT graph deals with concurrent add / remove of vertex and edges.
 * The content of vertex itself is not CRDT, it is on your call to deal with
 * concurrent modification. (The T parameter). You may use a CRDT type as the
 * T parameter. To have a fully CRDT update, you need to deal with dependency.
 * (Ex: receive update before the vertex is even created.). A solution is to
 * ad dependency metadata, another is to create temporary vertex (vertex created
 * with the minimal timestamp value).
 *
 * \warning
 * Timestamps are strictly unique with total order.
 * If (t1 == t2) is true, replicates may diverge.
 * (See quote and implementation).
 *
 *
 * \note
 * Quote from the CRDT article "A comprehensive study of CRDT":
 * "
 *  A Last-Writer-Wins [...] creates a total order of
 *  assignments by associating a timestamp with each update.
 *  Timestamps are assumed unique, totally ordered, and consistent with causal
 *  order; i.e., if assignment 1 happened-before assignment 2,
 *  the former's timestamp is less than the latter's. This may be implemented as
 *  a per-replicate counter concatenated with a unique replica identifier,
 *  such as its MAC address.
 * "
 *
 *
 * \tparam Key  Type of unique identifier for each graph vertex
 * \tparam T    Type of vertex content data.
 * \tparam U    Type of timestamps (Implements operators > and <).
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename T, typename U>
class LWWGraph {
    private:
        class Vertex;
        LWWMap<Key, Vertex, U> _adj;

        typedef typename LWWMap<Key,Vertex,U>::load_iterator load_iterator;


    // -------------------------------------------------------------------------
    // CRDT methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Query a vertex and its internal CRDT metadata.
         * Query a removed vertex returns this vertex with removed flag true.
         * This is meant to be used for CRDT uses such as update.
         *
         * \param key   Vertex's key to find.
         * \return      Iterator to the vertex or past-the-end if not found.
         */
        load_iterator queryVertex(const Key& key) {
            return _adj.query(key);
        }

        /**
         * Add a new vertex in the graph.
         *
         * If vertex already exists, update timestamps if was smaller.
         * This is required for CRDT properties and commutativity.
         *
         * \param key   The unique vertex's key.
         * \param stamp Timestamp of this operation.
         */
        void addVertex(const Key& key, const U& stamp) {
            _adj.add(key, stamp);
        }

        /**
         * TODO doc
         */
        void removeVertex(const Key& key, const U& stamp) {
            _adj.remove(key, stamp);
            // TODO IMPORTANT: Remove from all edge links
        }

        /**
         * Add edge from a vertex to another.
         *
         * \warning
         * This also performs a 'addVertex' for vertex 'from' and 'to'.
         * All operations must be commutative and this ensure addEdge is
         * commutative even if addEdge is received before the add operations.
         * If after the 'addVertex' operation, any vertex is still marked as
         * removed. Meaning 'addEdge' was before 'removeVertex', this edge
         * is marked as removed (With the 'removeVertex' timestamp).
         *
         * \param from  The origin vertex.
         * \param to    The destination vertex.
         */
        void addEdge(const Key& from, const Key& to, const U& stamp) {
            _adj.add(from, stamp);
            _adj.add(to, stamp);

            auto res = _adj.query(from);
            Vertex &v = res->second._content;
            v._edges.add(to, stamp);
        }

        /**
         * Remove the specific edge between two vertex.
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
            Vertex &v = res->second._content;
            v._edges.remove(to, stamp);
        }


    // -------------------------------------------------------------------------
    // Iterator
    // -------------------------------------------------------------------------

    public:

        /**
         * Returns a load iterator to the beginning.
         */
        load_iterator lbegin() {
            return _adj.lbegin();
        }

        /**
         * Returns a load iterator to the end.
         */
        load_iterator lend() {
            return _adj.lend();
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
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const LWWGraph& lhs, const LWWGraph& rhs) {
            return (lhs._adj == rhs._adj);
        }

        /**
         * Check if lhs and rhs are not equals.
         * See operator == for further information about equality meaning.
         *
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


/**
 * TODO doc
 */
template<typename Key, typename T, typename U>
class LWWGraph<Key,T,U>::Vertex {
    public:
        T               _content;
        LWWSet<Key,U>   _edges;

    public:
        friend bool operator==(const Vertex& lhs,const Vertex& rhs) {
            return (lhs._edges == rhs._edges) && (lhs._content == rhs._content);
        }
};


}} // End namespaces


