#pragma once
#include <iostream>

#include <unordered_map>
#include <utility> // std::pair
#include <cassert>
#include <ostream>

namespace CRDT {
namespace CmRDT {


/**
 * Last-Writer-Wins Unordered Set.
 * CmRDT (Operation-based)
 *
 * Associative container that contains unique keys.
 * Timestamps is assigned to each add / remove operation to create total order
 * of operations.
 *
 * \note
 * All operations on this set are commutative! You may receive a remove operation
 * even before its create operation.
 *
 * \note
 * Internally uses std::unordered_map (To store key and CRDT metadata).
 * Insertion, deletion and query follow at least std::unordered_map complexity.
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
        class const_iterator;
        class Metadata;

        typedef typename std::unordered_map<Key, Metadata>::const_iterator const_load_iterator;
        typedef typename std::unordered_map<Key, Metadata>::size_type size_type;

    private:
        std::unordered_map<Key, Metadata> _map;
        size_type _sizeAlive = 0; // Nb of alive elts (Not marked as removed)


    // -------------------------------------------------------------------------
    // Capacity methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Checks if the container has no 'alive' elements.
         *
         * \param true if the container is empty, false otherwise.
         */
        bool empty() const noexcept {
            return _sizeAlive == 0;
        }

        /**
         * Returns the number of 'alive' elements in the container.
         *
         * \return Number of alive elements in the container.
         */
        size_type size() const {
            return _sizeAlive;
        }

        /**
         * Returns the maximum number of elements the container is able to hold
         * due to system or library implementation limitations,
         * i.e. std::distance(begin(), end()) for the largest container.
         *
         * \return Maximum number of elements.
         */
        size_type max_size() const noexcept {
            return _map.max_size();
        }


    // -------------------------------------------------------------------------
    // Lookup methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Query a key and its internal CRDT metadata.
         * This means, query a removed key will return this key with its
         * internal CRDT metadata (isRemoved true). This may be useful
         * for operation such as updates done in higher level. (Since update
         * should be applied regardless key status.)
         *
         * If this key has never been added in set, returns past-the-end
         * (See end()) iterator.
         *
         * \param key The key to find.
         * \return Iterator to the key with CRDT metadata or end() if not found.
         */
        const_load_iterator query(const Key& key) const {
            return _map.find(key);
        }

        /**
         * Find a key in the container.
         * This only lookup for key that are not internally deleted.
         * (Like a normal set::find method).
         * If key is internally removed (removed flag to true), find return
         * past-the-end anyway (see end()).
         *
         * \param key Key value of the element to search for.
         * \return Iterator to the element with key or past-the-end if not found.
         */
        const_iterator find(const Key& key) const {
            auto elt_iterator = _map.find(key);
            if(elt_iterator != _map.end() && !elt_iterator->second._isRemoved) {
                const_iterator it(*this);
                it._it = elt_iterator;
                return it;
            }
            else {
                return this->end();
            }
        }


    // -------------------------------------------------------------------------
    // Modifiers methods
    // -------------------------------------------------------------------------

    public:

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
                    if(keyElt._isRemoved == true) {
                        ++_sizeAlive;
                    }
                    keyElt._timestamp = stamp;
                    keyElt._isRemoved = false;
                }
            }
            else {
                ++_sizeAlive;
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
            bool isKeyAdded = res.second;
            Metadata& keyElt= res.first->second;
            U keyStamp      = keyElt._timestamp;

            if(!isKeyAdded) {
                assert(keyStamp != stamp);
                if(keyStamp < stamp) {
                    if(keyElt._isRemoved == false) {
                        --_sizeAlive;
                    }
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
         * Returns a constant iterator to the beginning.
         */
        const_iterator begin() const noexcept {
            return const_iterator(*this);
        }

        /**
         * Returns a constant iterator to the end.
         */
        const_iterator end() const noexcept {
            const_iterator it(*this);
            it._it = _map.end();
            return it;
        }

        /**
         * Returns a constant load iterator to the beginning.
         *
         * \see load_iterator
         */
        const_load_iterator lbegin() const noexcept {
            return _map.begin();
        }

        /**
         * Returns a constant load iterator to the end.
         *
         * \see load_iterator
         */
        const_load_iterator lend() const noexcept {
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
            if(lhs.size() != rhs.size()) {
                return false;
            }

            // Dev note: in the worst case, this is N2 complexity.
            // Better solution? Maybe. For now I don't have time to think about.
            // Equality should not be called that often anyway (Since in
            // collab environment, the local user has one replicate).
            for(auto& elt : lhs) {
                if(rhs.find(elt) == rhs.end()){
                    return false;
                }
            }
            for(auto& elt: rhs) {
                if(lhs.find(elt) == lhs.end()) {
                    return false;
                }
            }
            return true;
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
 * Represents the CRDT internal state of a key in set.
 * Keys are never removed, only marked as removed with a timestamps.
 * See CRDT articles.
 */
template<typename Key, typename U>
class LWWSet<Key,U>::Metadata {
    public:
        U       _timestamp;
        bool    _isRemoved;
};


/**
 * Constant iterator for LWWSet.
 * Iterate over all keys that are in set and are NOT marked as removed.
 */
template<typename Key, typename U>
class LWWSet<Key,U>::const_iterator : public std::iterator<std::input_iterator_tag, Key> {
    private:
        friend LWWSet;
        const LWWSet& _data;
        typename std::unordered_map<Key, Metadata>::const_iterator _it;


    public:

        const_iterator(const LWWSet& set) : _data(set) {
            _it = _data._map.begin();

            // If first element is already removed, skip it
            while(_it != _data._map.end() && _it->second._isRemoved) {
                ++_it;
            }
        }

        const_iterator& operator++() {
            ++_it;

            while(_it != _data._map.end() && _it->second._isRemoved) {
                ++_it;
            }
            return *this;
        }

        bool operator==(const const_iterator& other) const {
            return _it == other._it;
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

        const Key& operator*() const {
            return _it->first;
        }
};


}} // End namespaces


