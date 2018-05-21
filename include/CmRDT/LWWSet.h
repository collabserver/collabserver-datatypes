#pragma once
#include <iostream>

#include <unordered_map>
#include <utility> // std::pair
#include <cassert>
#include <ostream>

namespace CRDT {
namespace CmRDT {


/**
 * Last-Writer-Wins Set (LWW Set).
 * CmRDT (Operation-based)
 *
 * Associative container that contains unique keys.
 * Timestamps is assigned to each add / remove operation to create total order
 * of operations.
 *
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
 * \tparam Key  Type of set elements.
 * \tparam U    Type of timestamps (Implements operators > and <).
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename U>
class LWWSet {
    public:
        class iterator;
        class Metadata;

        typedef typename std::unordered_map<Key, Metadata>::iterator load_iterator;

    private:
        std::unordered_map<Key, Metadata> _map;


    // -------------------------------------------------------------------------
    // CRDT methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Query a key and its internal CRDT metadata.
         * This means, query on a removed key will return this key with 
         * removed metadata to true. This may be useful to have CRDT update
         * (Query will return the key, whereas it has been deleted or not).
         *
         * If this key has never been added in set, returns past-the-end
         * (See end()) iterator.
         *
         * \param key The key to find.
         * \return Iterator to the key with CRDT metadata or end() if not found.
         */
        load_iterator query(const Key& key) {
            return _map.find(key);
        }

        /**
         * Inserts new key in the container.
         *
         * Add a new key into the container.
         * If key already exists, update timestamps if was smaller than this one.
         * This is required for CRDT properties and commutativity.
         *
         * \param key   Key element to add.
         * \param stamp Timestamps of this operation.
         */
        void add(const Key& key, const U& stamp) {
            Metadata elt; // Content is not set here
            elt._timestamp  = stamp;
            elt._isRemoved  = false;

            auto res        = _map.insert(std::make_pair(key, elt));
            bool isKeyAdded = res.second;
            Metadata& keyElt= res.first->second;
            U keyStamp      = keyElt._timestamp;

            if(!isKeyAdded) {
                assert(keyStamp != stamp);
                if(keyStamp < stamp) {
                    keyElt._timestamp = stamp;
                    keyElt._isRemoved = false;
                }
            }
        }

        /**
         * Remove a key from the container.
         *
         * If key doesn't exists, internally add it first (with removed flag).
         * This is because remove / add are commutative and remove may be
         * received before add.
         *
         * \param key   Key of the element to add.
         * \param stamp Timestamps of this operation.
         */
        void remove(const Key& key, const U& stamp) {
            Metadata elt; // Content is not set here
            elt._timestamp  = stamp;
            elt._isRemoved  = true;

            auto res        = _map.insert(std::make_pair(key, elt));
            bool isKeyAdded   = res.second;
            Metadata& keyElt= res.first->second;
            U keyStamp      = keyElt._timestamp;

            if(!isKeyAdded) {
                assert(keyStamp != stamp);
                if(keyStamp < stamp) {
                    keyElt._timestamp = stamp;
                    keyElt._isRemoved = true;
                }
            }
        }


    // -------------------------------------------------------------------------
    // Iterators
    // -------------------------------------------------------------------------

    public:

        /**
         * Returns an iterator to the beginning.
         */
        iterator begin() {
            return iterator(*this);
        }

        /**
         * Returns an iterator to the end.
         */
        iterator end() {
            iterator it(*this);
            it._it = _map.end();
            return it;
        }

        /**
         * Returns a load iterator to the beginning.
         *
         * \see load_iterator
         */
        load_iterator lbegin() {
            return _map.begin();
        }

        /**
         * Returns a load iterator to the end.
         *
         * \see load_iterator
         */
        load_iterator lend() {
            return _map.end();
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if lhs and rhs are equals.
         * Two sets are equal if their 'living' set of key are equal.
         *
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const LWWSet& lhs, const LWWSet& rhs) {
            // TODO This is not doing the actual job said in the doc.
            return (lhs._map == rhs._map);
        }

        /**
         * Check if lhs and rhs are not equals.
         * See operator == for further information about equality meaning.
         *
         * \return True if not equal, otherwise, return false.
         */
        friend bool operator!=(const LWWSet& lhs, const LWWSet& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Display the internal map content.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out, const LWWSet<Key,U>& o) {
            out << "CmRDT::LWWSet = ";
            for(const auto& elt : o._map) {
                out << "(K=" << elt.first
                    << ",U=" << elt.second._timestamp;
                if(elt.second._isRemoved == true) {
                    out << ",removed) ";
                }
                else {
                    out << ",alive) ";
                }
            }
            return out;
        }
};


// *****************************************************************************
// Nested classes
// *****************************************************************************


/**
 * Represents the CRDT state of the elements.
 */
template<typename Key, typename U>
class LWWSet<Key,U>::Metadata {
    public:
        U       _timestamp;
        bool    _isRemoved;

    public:
        friend bool operator==(const Metadata& lhs, const Metadata& rhs) {
            // Required for equality test of maps.
            // We only care about the keys.
            // Two key with different metadata are still same from the
            // end user point of view.
            return true;
        }

        friend bool operator!=(const Metadata& lhs, const Metadata& rhs) {
            return !(lhs == rhs);
        }
};


/**
 * Iterator for LWWSet.
 * Iterate over all keys that are in set and are NOT marked as removed.
 */
template<typename Key, typename U>
class LWWSet<Key,U>::iterator : public std::iterator<std::input_iterator_tag, Key> {
    private:
        friend LWWSet;
        typedef typename std::unordered_map<Key, Metadata>::iterator internal_iterator;

        LWWSet& _data;
        internal_iterator _it;


    public:

        iterator(LWWSet& set) : _data(set) {
            _it = _data._map.begin();

            // If first element is already removed, skip it
            while(_it != _data._map.end() && _it->second._isRemoved) {
                ++_it;
            }
        }

        iterator& operator++() {
            ++_it;

            while(_it != _data._map.end() && _it->second._isRemoved) {
                ++_it;
            }
            return *this;
        }

        bool operator==(const iterator& other) const {
            return _it == other._it;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        const Key& operator*() {
            // TODO: remove this const (But _it is const_iterator atm.. :/ )
            return _it->first;
        }
};


}} // End namespaces


