#pragma once

#include <ostream>

#include "GMap.h"
#include "GSet.h"

namespace collab {
namespace CvRDT {


/**
 * \deprecated
 * I originally made this class as an example and is not fully tested.
 * Several update / bug fixing may be required to use it in real world.
 *
 *
 * Grow-only Graph.
 * CvRDT (State-based).
 *
 * \warning
 * Data type T must work with std::max (is_arithmetic == true)
 *
 *
 * \tparam T Data type on a vertex
 * \tparam K ID type (unique for each vertex).
 */
template<typename K, typename T>
class GGraph {

    // TODO: To cleanup
    private:

        struct _Vertex {
            public:
            const K _id;
            T _content;
            GSet<K> _edges;

            friend bool operator==(const _Vertex& lhs, const _Vertex& rhs) {
                return (lhs._id == rhs._id) && (lhs._content == rhs._content);
            }

            friend bool operator!=(const _Vertex& lhs, const _Vertex& rhs) {
                return !(lhs == rhs);
            }

            friend bool operator<(const _Vertex& lhs, const _Vertex& rhs) {
                return lhs._content < rhs._content;
            }

            _Vertex& operator=(const _Vertex& other) {
                _content = other._content;
                _edges = other._edges;
                return *this;
            }

            friend bool operator>(const _Vertex& lhs, const _Vertex& rhs) {
                return !(lhs < rhs);
            }
        };

        GMap<K,_Vertex> _adj;


    public:
        typedef typename GMap<K,_Vertex>::const_iterator    const_iterator;


    public:

        void addVertex(const K& id, const T& value) {
            _adj.insert({id, {id, value, {}}});
        }

        void addEdge(const K& src, const K& dest) {
            if(src == dest) {
                return;
            }
            if(_adj.count(src) == 1 && _adj.count(dest) == 1) {
                _Vertex& v = _adj[src];
                v._edges.insert(dest);
            }
        }

        void merge(const GGraph<K,T>& other) {
            for(const_iterator it = other._adj.cbegin(); it != other._adj.cend(); ++it) {
                const K& id = it->first;
                const _Vertex& v = it->second;
                if(_adj.count(id) == 0) {
                    _adj.insert({id, {id, v._content, v._edges}});
                }
                else {
                    _adj[id]._edges.merge(v._edges);
                    _adj[id]._content = std::max(_adj[id]._content, v._content);
                }
            }
        }

    public:

        friend bool operator==(const GGraph& lhs, const GGraph& rhs) {
            return lhs._adj == rhs._adj;
        }

        friend bool operator!=(const GGraph& lhs, const GGraph& rhs) {
            return !(lhs == rhs);
        }

        friend std::ostream& operator<<(std::ostream& out,
                                        const GGraph<K,T>& o) {
            out << "GGraph = {\n";
            for(const_iterator it = o._adj.cbegin(); it != o._adj.cend(); ++it) {
                _Vertex v = it->second;
                out << it->first << " (" << v._content << ") -> ";

                //info: auto is typename GSet<K>::const_iterator
                for(auto itv = v._edges.cbegin(); itv != v._edges.cend(); ++itv) {
                    out << *itv << " ";
                }
                out << "\n";
            }
            out << "}";
            return out;
        }
};


}} // End namespaces


