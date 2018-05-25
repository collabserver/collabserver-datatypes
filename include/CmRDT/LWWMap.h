#pragma once

#include <unordered_map>
#include <utility> // std::pair
#include <cassert>
#include <ostream>
#include <iostream>

namespace CRDT {
namespace CmRDT {


/**
 * Last-Writer-Wins Map (LWW Map).
 * CmRDT (Operation-based)
 *
 * Associative container that contains key-value pairs with unique keys.
 * Timestamps is assigned to each add / remove operation to create total order
 * of operations. Internally, remove operation does not actually remove
 * the key-value but only mark them as deleted.
 *
 * \note
 * All operations on this container are commutative! You may receive 'remove'
 * operation even before 'create' operation.
 *
 * \note
 * Internally uses std::unordered_map (To store key and CRDT metadata).
 * Insertion, deletion and query follow at least std::unordered_map complexity.
 * \see http://en.cppreference.com/w/cpp/container/unordered_map
 *
 * \warning
 * CRDT Map only deals with concurrent add / remove of keys. The key's content
 * by itself is not CRDT. It is on your call to deal with the content.
 * You may for instance use a LWWRegister. (In such case, after any add,
 * call register update function to also update, if necessary, the content).
 *
 *
 * \warning
 * Timestamps are strictly unique with total order.
 * If (t1 == t2) is true, replicates may diverge.
 * (See quote and implementation).
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
 * \tparam Key  Type of key.
 * \tparam T    Type of element.
 * \tparam U    Type of timestamps (Must implements operators > and <).
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename T, typename U>
class LWWMap {
    public:
        class Element;
        class iterator;
        class const_iterator;

        typedef typename std::unordered_map<Key, Element>::iterator crdt_iterator;
        typedef typename std::unordered_map<Key, Element>::const_iterator const_crdt_iterator;
        typedef typename std::unordered_map<Key, Element>::size_type size_type;

        // From outside, we see LWWMap as <Key, T> (Except crdt_iterator)
        typedef typename std::unordered_map<Key, T>::key_type        key_type;
        typedef typename std::unordered_map<Key, T>::mapped_type     mapped_type;
        typedef typename std::unordered_map<Key, T>::value_type      value_type;
        typedef typename std::unordered_map<Key, T>::reference       reference;
        typedef typename std::unordered_map<Key, T>::const_reference const_reference;
        typedef typename std::unordered_map<Key, T>::pointer         pointer;
        typedef typename std::unordered_map<Key, T>::const_pointer   const_pointer;

    private:
        std::unordered_map<Key, Element> _map;
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
        size_type size() const noexcept {
            return _sizeAlive;
        }

        /**
         * Returns the maximum number of elements the container is able to hold
         * due to system or library implementation limitations,
         * i.e. std::distance(begin(), end()) for the largest container.
         *
         * \see http://en.cppreference.com/w/cpp/container/unordered_map/max_size
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
         * Query a key-element and its internal CRDT metadata.
         * This means, query on a removed key will return this key with 
         * removed metadata to true. This may be useful to have CRDT update
         * (Query will return the key-elt, whereas it has been deleted or not).
         *
         * If this key has never been added in set, returns past-the-end
         * (See lend()) iterator.
         *
         * \param key The key to find.
         * \return Iterator to the key-element or past-the-end if not found.
         */
        crdt_iterator query(const Key& key) {
            return _map.find(key);
        }

        /**
         * Find a key-element in the container.
         * This only lookup for key that are not internally deleted.
         * (Like a normal map::find method).
         * If element is internally removed (removed flag to true), find return
         * past-the-end anyway (see end()).
         *
         * \param key Key value of the element to search for.
         * \return Iterator to the element with key or past-the-end if not found.
         */
        iterator find(const Key& key) {
            auto elt_iterator = _map.find(key);
            if(elt_iterator != _map.end() && !elt_iterator->second.isRemoved()) {
                iterator it(*this);
                it._it = elt_iterator;
                return it;
            }
            else {
                return this->end();
            }
        }

        const_iterator find(const Key& key) const {
            auto elt_iterator = _map.find(key);
            if(elt_iterator != _map.end() && !elt_iterator->second.isRemoved()) {
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
         * \param key   Key of the element to add.
         * \param stamp Timestamps of this operation.
         */
        void add(const Key& key, const U& stamp) {
            Element elt(key); // Content is not set here
            elt._timestamp  = stamp;
            elt._isRemoved  = false;

            auto res        = _map.insert(std::make_pair(key, elt));
            bool keyAdded   = res.second;
            Element& keyElt = res.first->second;
            U keyStamp      = keyElt.timestamp();

            if(!keyAdded) {
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
         * If key doesn't exists, add it first with removed state true.
         * This is because remove / add are commutative and remove may be
         * received before add.
         *
         * \param key   Key of the element to add.
         * \param stamp Timestamps of this operation.
         */
        void remove(const Key& key, const U& stamp) {
            Element elt(key); // Content is not set here
            elt._timestamp  = stamp;
            elt._isRemoved  = true;

            auto res        = _map.insert(std::make_pair(key, elt));
            bool keyAdded   = res.second;
            Element& keyElt = res.first->second;
            U keyStamp      = keyElt.timestamp();

            if(!keyAdded) {
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
    // Hash policy methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Change capacity of the container.
         * See cpp std::unordered_set::reserve
         *
         * \param count New capacity of the container.
         */
        void reserve(size_type count) {
            _map.reserve(count);
        }


    // -------------------------------------------------------------------------
    // Iterator
    // -------------------------------------------------------------------------

    public:

        /**
         * Returns iterator to the beginning.
         */
        iterator begin() {
            return iterator(*this);
        }

        /**
         * Returns iterator to the end.
         */
        iterator end() {
            iterator it(*this);
            it._it = _map.end();
            return it;
        }

        /**
         * Returns constant iterator to the beginning.
         */
        const_iterator begin() const noexcept {
            return const_iterator(*this);
        }

        /**
         * Returns const iterator to the end.
         */
        const_iterator end() const noexcept {
            const_iterator it(*this);
            it._it = _map.end();
            return it;
        }

        /**
         * Returns constant iterator to the beginning.
         */
        const_iterator cbegin() const noexcept {
            return const_iterator(*this);
        }

        /**
         * Returns constant iterator to the end.
         */
        const_iterator cend() const noexcept {
            const_iterator it(*this);
            it._it = _map.end();
            return it;
        }

        /**
         * Returns a crdt iterator to the beginning.
         */
        crdt_iterator lbegin() noexcept {
            return _map.begin();
        }

        /**
         * Returns a crdt iterator to the end.
         */
        crdt_iterator lend() noexcept {
            return _map.end();
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if lhs and rhs are equals.
         * Two LWWMap are equal if their map of 'living' keys are equal.
         *
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const LWWMap& lhs, const LWWMap& rhs) {
            if(lhs.size() != rhs.size()) {
                return false;
            }

            // Dev note: in the worst case, this is N2 complexity.
            // See note in LWWSet
            for(auto& elt : lhs) {
                auto other = rhs.find(elt.first);;
                if(other == rhs.cend()){
                    return false;
                }
                if(elt.second != other->second) {
                    return false;
                }
            }
            for(auto& elt: rhs) {
                auto other = lhs.find(elt.first);
                if(other == lhs.cend()) {
                    return false;
                }
                if(elt.second != other->second) {
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
        friend bool operator!=(const LWWMap& lhs, const LWWMap& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Display the internal map content.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out,
                                        const LWWMap<Key,T,U>& o) {
            out << "CmRDT::LWWMap = ";
            for(const auto& elt : o._map) {
                out << "(K=" << elt.first
                    << ",T=" << elt.second.value()
                    << ",U=" << elt.second.timestamp();
                if(elt.second.isRemoved()) {
                    out << ",removed) ";
                }
                else {
                    out << ",alive) ";
                }
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
 * Map cell component.
 */
template<typename Key, typename T, typename U>
class LWWMap<Key, T, U>::Element {

    private:
        friend LWWMap;

        // I did this for the iterator* method
        // This is possibly not the best solution
        std::pair<const Key,T> _internalValue;

        U       _timestamp;
        bool    _isRemoved;

    public:
        Element(const Key key) : _internalValue(std::make_pair(key, T{})) {
        }

    public:
        T& value() {
            return _internalValue.second;
        }

        const T& value() const {
            return _internalValue.second;
        }

        const U& timestamp() const {
            return _timestamp;
        }

        bool isRemoved() const {
            return _isRemoved;
        }
};


/**
 * Iterator for LWWMap container.
 * Iterate over all keys-elements that are in set and are NOT marked as removed.
 */
template<typename Key, typename T, typename U>
class LWWMap<Key, T, U>::iterator : public std::iterator<std::input_iterator_tag, value_type> {

    private:
        friend LWWMap;

        LWWMap&       _data;
        crdt_iterator _it;

    public:
        iterator(LWWMap& map) : _data(map) {
            _it = _data._map.begin();

            // If first element is already removed, skip it
            while(_it != _data._map.end() && _it->second.isRemoved()) {
                ++_it;
            }
        }

        iterator& operator++() {
            ++_it;

            while(_it != _data._map.end() && _it->second.isRemoved()) {
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

        reference operator*() const {
            return _it->second._internalValue;
        }

        pointer operator->() const {
            return &(_it->second._internalValue);
        }
};


/**
 * Constant iterator for LWWMap container.
 * Iterate over all keys-elements that are in set and are NOT marked as removed.
 */
template<typename Key, typename T, typename U>
class LWWMap<Key, T, U>::const_iterator : public std::iterator<std::input_iterator_tag, value_type> {

    private:
        friend LWWMap;

        const LWWMap&       _data;
        const_crdt_iterator _it;

    public:
        const_iterator(const LWWMap& map) : _data(map) {
            _it = _data._map.begin();

            // If first element is already removed, skip it
            while(_it != _data._map.end() && _it->second.isRemoved()) {
                ++_it;
            }
        }

        const_iterator& operator++() {
            ++_it;

            while(_it != _data._map.end() && _it->second.isRemoved()) {
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

        const_reference operator*() const {
            return _it->second._internalValue;
        }

        const_pointer operator->() const {
            return &(_it->second._internalValue);
        }
};


}} // End namespace


