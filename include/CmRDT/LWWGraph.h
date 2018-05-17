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


    // -------------------------------------------------------------------------
    // CRDT methods
    // -------------------------------------------------------------------------

    public:

        /**
         * TODO doc
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
         * TODO doc
         */
        void addEdge(const Key& from, const Key& to, const U& stamp) {
            if(from == to) {
                return;
            }
            _adj.add(from, stamp);
            _adj.add(to, stamp);

            Vertex &v = _adj.query(from);
            v._edges.add(to, stamp);
        }

        /**
         * TODO doc
         */
        void removeEdge(const Key& from, const Key& to, const U& stamp) {
            if(from == to) {
                return;
            }
            _adj.remove(from, 0);
            _adj.remove(to, 0);

            Vertex &v = _adj.query(from);
            v._edges.remove(to, stamp);
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if lhs and rhs are equals.
         * Two LWWGraph are equal if their internal content are equal.
         * Removed elements are used to determine equality.
         *
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const LWWGraph& lhs, const LWWGraph& rhs) {
            return lhs._adj == rhs._adj;
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
            // TODO
            return out;
        }
};


/**
 * TODO doc
 */
template<typename Key, typename T, typename U>
class LWWGraph<Key,T,U>::Vertex {
    public:
        Key             _id;
        T               _content;
        LWWSet<Key,U>   _edges;

    public:
        friend bool operator==(const Vertex& lhs,const Vertex& rhs) {
            return (lhs._edges == rhs._edges) && (lhs._content == rhs._content);
        }
};


}} // End namespaces


