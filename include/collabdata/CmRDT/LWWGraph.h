#pragma once

#include <ostream>
#include <cassert>
#include <type_traits>

#include "LWWMap.h"
#include "LWWSet.h"

namespace collab {
namespace CmRDT {


/**
 * Information used by add_edge method.
 * Describe return status of add_edge. (LWWGraph)
 *
 * \see LWWGraph
 */
struct AddEdgeInfo {
    bool isEdgeAdded;
    bool isFromAdded;
    bool isToAdded;
};


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
 * Any added key is never removed but only marked as deleted instead.
 * For that reason, this CRDT container may not fit all systems
 * due to the used memory space.
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
 * T type must have a default constructor.
 * U timestamp must accept "U t = {0}" (This should set with minimal value).
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
    public:
        class Vertex;

        typedef typename LWWMap<Key,Vertex,U>::size_type        size_type;
        typedef typename LWWMap<Key,Vertex,U>::iterator         iterator;
        typedef typename LWWMap<Key,Vertex,U>::const_iterator   const_iterator;
        typedef typename LWWMap<Key,Vertex,U>::crdt_iterator    crdt_iterator;
        typedef typename LWWMap<Key,Vertex,U>::const_crdt_iterator
            const_crdt_iterator;

    private:
        LWWMap<Key, Vertex, U> _adj;


    // -------------------------------------------------------------------------
    // Capacity methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Checks if the graph has no vertex.
         * Only elements that are not marked as 'removed' count.
         *
         * \return True if the container is empty, false otherwise.
         */
        bool empty() const noexcept {
            return _adj.empty();
        }

        /**
         * Checks if the graph has no vertex.
         * Also check for elements marked as 'removed'.
         *
         * \return True if the container is empty, false otherwise.
         */
        bool crdt_empty() const noexcept {
            return _adj.crdt_empty();
        }

        /**
         * Returns the number of vertex in this graph.
         * Only elements that are not marked as 'removed' count.
         *
         * \return Number of elements in the container.
         */
        size_type size() const noexcept {
            return _adj.size();
        }

        /**
         * Returns the number of vertex in this graph.
         * Also check for elements marked as 'removed'.
         *
         * \return Number of elements in the container.
         */
        size_type crdt_size() const noexcept {
            return _adj.crdt_size();
        }


    // -------------------------------------------------------------------------
    // Lookup methods (Vertex)
    // -------------------------------------------------------------------------

    public:

        /**
         * Find a vertex in the graph.
         *
         * This only lookup for vertex that are not internally deleted.
         * If element is internally removed (removed flag is true), find returns
         * past-the-end anyway (see end()).
         *
         * \param key   Vertex's key to find.
         * \return      Iterator to the vertex or end() if not found.
         */
        iterator find_vertex(const Key& key) {
            return _adj.find(key);
        }

        /**
         * \copydoc LWWGraph::find_vertex
         */
        const_iterator find_vertex(const Key& key) const {
            return _adj.find(key);
        }

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
         * \return      CRDT iterator to the vertex or crdt_end() if not found.
         */
        crdt_iterator crdt_find_vertex(const Key& key) {
            return _adj.crdt_find(key);
        }

        /**
         * \copydoc LWWGraph::crdt_find_vertex
         */
        const_crdt_iterator crdt_find_vertex(const Key& key) const {
            return _adj.crdt_find(key);
        }

        /**
         * Count the number of vertex with this key.
         * Since no duplicate are allowed, return 0 or 1.
         *
         * \param key Key value of the element to count.
         * \return Number of elements with this key, either 0 or 1.
         */
        size_type count_vertex(const Key& key) const {
            return _adj.count(key);
        }

        /**
         * Count the number of vertex with this key.
         * Since no duplicate are allowed, return 0 or 1.
         * Also lookup for element internally marked as 'removed'.
         *
         * \param key Key value of the element to count.
         * \return Number of elements with this key, either 0 or 1.
         */
        size_type crdt_count_vertex(const Key& key) const {
            return _adj.crdt_count(key);
        }


    // -------------------------------------------------------------------------
    // Lookup methods (Edges)
    // -------------------------------------------------------------------------

    public:

        /**
         * Count the number of edge from given vertex to another.
         * Since no duplicate are allowed, return 0 or 1.
         * Only elements that are not marked as 'removed' count.
         *
         * \param from  The origin vertex.
         * \param to    The destination vertex.
         * \return Number of edges with this characteristic, either 0 or 1.
         */
        size_type count_edge(const Key& from, const Key& to) const {
            auto vertex_it = _adj.find(from);
            if(vertex_it != _adj.end()) {
                return vertex_it->second.edges().count(to);
            }
            return 0;
        }

        /**
         * Count the number of edge from given vertex to another.
         * Since no duplicate are allowed, return 0 or 1.
         * Also check for elements marked as 'removed'. (Internal CRDT data).
         *
         * \param from  The origin vertex.
         * \param to    The destination vertex.
         * \return Number of edges with this characteristic, either 0 or 1.
         */
        size_type crdt_count_edge(const Key& from, const Key& to) const {
            auto vertex_it = _adj.crdt_find(from);
            if(vertex_it != _adj.crdt_end()) {
                return vertex_it->second.value().edges().crdt_count(to);
            }
            return 0;
        }


    // -------------------------------------------------------------------------
    // Modifiers methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Add a new vertex in the graph.
         *
         * If key already exists, use timestamps for concurrency control.
         *
         * \par Concurrent add_vertex / add_vertex
         * Timestamp is updated with the higher value. Key is added in any case.
         * Returns false. This is because, it only updates timestamp of the
         * operation. Key was already added in container before the operation.
         *
         * \par Concurrent add_vertex / remove_vertex
         * Uses the higher timestamp select the winning operation.
         * If remove timestamp wins, this add operation does nothing.
         * Otherwise, add is applied and true is returned.
         *
         * \par Idempotent
         * Duplicate calls with same stamp is idempotent.
         *
         * \note
         * This only adds the key. A default vertex is created.
         * To add key and set its content, call this add method and query the
         * newly added vertex (Then update it with its default value).
         *
         * \param key   The unique vertex's key.
         * \param stamp Timestamp of this operation.
         * \return True if vertex added, otherwise, return false.
         */
        bool add_vertex(const Key& key, const U& stamp) {
            return _adj.add(key, stamp);
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
         * \par Concurrent add_edge / remove_vertex
         * See documentation of LWWGraph::add_edge
         *
         * \par Idempotent
         * Duplicate calls with same stamp is idempotent.
         *
         * \see LWWGraph::add_edge
         *
         * \param key   The unique vertex's key.
         * \param stamp Timestamp of this operation.
         * \return True if vertex removed, otherwise, return false.
         */
        bool remove_vertex(const Key& key, const U& stamp) {
            bool isVertexRemoved = _adj.remove(key, stamp);

            // Remove all vertex's edges
            auto from_it = _adj.crdt_find(key);
            auto& edges = from_it->second.value()._edges;
            for(auto it = edges.crdt_begin(); it != edges.crdt_end(); ++it) {
                edges.remove(it->first, stamp);
            }

            // Remove all edge to this vertex (On others vertex)
            for(auto it = _adj.crdt_begin(); it != _adj.crdt_end(); ++it) {
                auto& edges = it->second.value()._edges;
                if(it->first != key && edges.count(key) == 1) {
                    edges.remove(key, stamp);
                }
            }

            return isVertexRemoved;
        }

        /**
         * Add edge from a vertex to another.
         *
         * This also performs a 'add_vertex' for vertex 'from' and 'to'.
         * This ensure add_edge is commutative even if add_edge is received
         * before the add operations.
         *
         * \par Concurrent add_edge / remove_vertex
         * If after the 'add_vertex' operation, any vertex is still marked as
         * removed. Meaning 'add_edge' was before 'remove_vertex', this edge
         * is marked as removed (With the 'remove_vertex' timestamp).
         * This resolve the concurrent 'add_edge' | 'remove_vertex'
         *
         * \par Idempotent
         * Duplicate calls with same stamp is idempotent.
         *
         * \param from  The origin vertex.
         * \param to    The destination vertex.
         * \return Structure to know if edge, from and/or, to where added.
         */
        AddEdgeInfo add_edge(const Key& from, const Key& to, const U& stamp) {
            AddEdgeInfo info;
            info.isFromAdded = _adj.add(from, stamp);
            info.isToAdded = false;
            if(from != to) {
                info.isToAdded = _adj.add(to, stamp);
            }

            auto from_it = _adj.crdt_find(from);
            Vertex& vertex = from_it->second.value();
            bool isAdded = vertex._edges.add(to, stamp);

            // If edge added, check whether vertex from or to are not removed.
            // If one of them is removed, this newly created edge must be
            // removed now. (important for CRDT commutativity)

            auto from_edge_it = vertex.edges().crdt_find(to);
            if(!from_edge_it->second.isRemoved()) {
                auto vertex_it_from = _adj.crdt_find(from);
                auto vertex_it_to = _adj.crdt_find(to);
                assert(vertex_it_from != _adj.crdt_end());
                assert(vertex_it_to != _adj.crdt_end());
                const bool from_removed = vertex_it_from->second.isRemoved();
                const bool to_removed = vertex_it_to->second.isRemoved();

                if(from_removed || to_removed) {
                    U from_time = vertex_it_from->second.timestamp();
                    U to_time   = vertex_it_to->second.timestamp();
                    U high_time  = (from_time > to_time) ? from_time : to_time;

                    vertex._edges.remove(to, high_time);
                    info.isEdgeAdded = false;
                    return info;
                }
            }
            info.isEdgeAdded = isAdded;
            return info;
        }

        /**
         * Removes the specific edge between two vertex.
         *
         * If from and/or to doesn't exists, create one with minimum possible
         * timestamp. This create a temporary vertex. This is use in case of
         * remove_edge is applied before 'add_edge'. (Required for CRDT
         * commutativity).
         *
         * \par Idempotent
         * Duplicate calls with same stamp is idempotent.
         *
         * \param from  The origin vertex.
         * \param to    The destination vertex.
         * \return True if edge removed, otherwise, return false.
         */
        bool remove_edge(const Key& from, const Key& to, const U& stamp) {
            _adj.remove(from, 0);
            if(from != to) {
                _adj.remove(to, 0);
            }

            auto coco_it = _adj.crdt_find(from);
            Vertex &v = coco_it->second.value();
            return v._edges.remove(to, stamp);
        }


    // -------------------------------------------------------------------------
    // Hash policy methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Change capacity of the vertex container.
         * This reserve capacity for the internal adjacency list.
         * (Edge list for each vertex is not reserved, only vertex list).
         *
         * \see http://en.cppreference.com/w/cpp/container/unordered_map/reserve
         *
         * \param count New capacity of the container.
         */
        void reserve(size_type count) {
            _adj.reserve(count);
        }


    // -------------------------------------------------------------------------
    // CRDT Specific
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if tow containers have the exact same internal data.
         * Element with removed flag are used for this comparison.
         *
         * \param other Container to compare with.
         * \return True if equals, otherwise, return false.
         */
        bool crdt_equal(const LWWGraph& other) const {
            if(_adj.crdt_equal(other._adj) == false) {
                return false;
            }

            // At this point only edges not marked as deleted are checked.
            // This is because crdt_equal calls == on each vertex, so edges==
            // is called instead of edges.crdt_equal()

            for(auto it = _adj.crdt_begin(); it != _adj.crdt_end(); ++it) {
                const auto& edges = it->second.value().edges();
                const auto other_it = other._adj.crdt_find(it->first);
                const auto& other_edges = other_it->second.value().edges();
                if(!edges.crdt_equal(other_edges)) {
                    return false;
                }
            }
            return true;
        }


    // -------------------------------------------------------------------------
    // Iterator
    // -------------------------------------------------------------------------

    public:

        /**
         * Returns iterator to the beginning.
         *
         * \return iterator to the first element.
         */
        iterator begin() {
            return iterator(_adj);
        }

        /**
         * Returns iterator to the end.
         *
         * \return iterator to the last element.
         */
        iterator end() {
            iterator it(_adj);
            it._it = _adj.end();
            return it;
        }

        /**
         * \copydoc LWWGraph::begin
         */
        const_iterator begin() const noexcept {
            return const_iterator(_adj);
        }

        /**
         * \copydoc LWWGraph::end
         */
        const_iterator end() const noexcept {
            const_iterator it(_adj);
            it._it = _adj.end();
            return it;
        }

        /**
         * \copydoc LWWGraph::begin
         */
        const_iterator cbegin() const noexcept {
            return const_iterator(_adj);
        }

        /**
         * \copydoc LWWGraph::end
         */
        const_iterator cend() const noexcept {
            const_iterator it(_adj);
            it._it = _adj.end();
            return it;
        }

        /**
         * Returns crdt_iterator to the beginning.
         *
         * \see LWWGraph::crdt_iterator
         * \return CRDT iterator to the first vertex.
         */
        crdt_iterator crdt_begin() {
            return _adj.crdt_begin();
        }

        /**
         * Returns crdt_iterator to the end.
         *
         * \see LWWGraph::crdt_iterator
         * \return CRDT iterator to the last vertex.
         */
        crdt_iterator crdt_end() {
            return _adj.crdt_end();
        }

        /**
         * \copydoc LWWGraph::crdt_begin
         */
        const_crdt_iterator crdt_begin() const {
            return _adj.crdt_begin();
        }

        /**
         * \copydoc LWWGraph::crdt_end
         */
        const_crdt_iterator crdt_end() const {
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
            out << "CmRDT::LWWGraph = ";
            for(auto it = o.crdt_begin(); it != o.crdt_end(); ++it) {
                out << "\n Vertex("
                    << it->first << ","
                    << it->second.timestamp();
                if(it->second.isRemoved()) {
                    out << ",x)";
                }
                else {
                    out << ",o)";
                }
                out << " -> " << it->second.value().edges();
            }
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
         * \copydoc Vertex::content
         */
        const T& content() const {
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
         * \copydoc Vertex::edges
         */
        const LWWSet<Key,U>& edges() const {
            return _edges;
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

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


