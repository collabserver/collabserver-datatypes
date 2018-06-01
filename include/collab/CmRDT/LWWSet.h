#pragma once

#include <unordered_map>
#include <utility> // std::pair
#include <cassert>
#include <ostream>

namespace collab {
namespace CmRDT {


/**
 * \brief
 * Last-Writer-Wins Set.
 * CmRDT (Operation-based)
 *
 * Associative container that contains a set of unique keys.
 * Internally uses std::unordered_map to store the key and its CRDT metadata.
 * As the end user, you see this container as a std::unordered_set (See iterator
 * for instance). You may request the actual internal data using crdt_iterator.
 * Check out std::unordered_set documentation for further informations.
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
 * Timestamps are strictly unique with total order.
 * If (t1 == t2) is true, replicates may diverge.
 * (See quote and implementation for further informations).
 *
 * \warning
 * U timestamp must accept "U t = {0}".
 * This must set with minimal value.
 *
 * \see http://en.cppreference.com/w/cpp/container/unordered_set
 * \see http://en.cppreference.com/w/cpp/container/unordered_map
 *
 *
 * \tparam Key  Type of set elements.
 * \tparam U    Type of timestamps (Must implements operators > and <).
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename U>
class LWWSet {
    public:
        class const_iterator;
        class Metadata;

        typedef typename std::unordered_map<Key, Metadata>::size_type size_type;
        typedef typename std::unordered_map<Key, Metadata>::const_iterator
            const_crdt_iterator;

    private:
        std::unordered_map<Key, Metadata> _map;
        size_type _sizeAlive = 0; // Nb of alive elts (Not marked as removed)


    // -------------------------------------------------------------------------
    // Capacity methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Checks if the container has no elements.
         * Only elements that are not marked as 'removed' count.
         *
         * \return True if the container is empty, false otherwise.
         */
        bool empty() const noexcept {
            return _sizeAlive == 0;
        }

        /**
         * Returns the number of elements in the container.
         * Only elements that are not marked as 'removed' count.
         *
         * \return Number of elements in the container.
         */
        size_type size() const noexcept {
            return _sizeAlive;
        }

        /**
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
         * Query a key and its internal CRDT metadata.
         *
         * If element with this key exists in the internal container, it is
         * returned, regardless its 'removed' status.
         * This may be useful for datatypes built on top of this set.
         * To have CRDT updates, you must apply the update, regardless its
         * internal status. This query will return the element in any case
         * (If already added once in the container.)
         *
         * If this key has never been added in set, returns crdt past-the-end
         * (See crdt_end()) iterator.
         *
         * \param key The key to query.
         * \return Iterator CRDT to the key or crdt_end() if not found.
         */
        const_crdt_iterator query(const Key& key) const {
            return _map.find(key);
        }

        /**
         * Find a key in the container.
         *
         * This only lookup for keys that are not internally deleted.
         * (Like a normal set::find method).
         * If key is internally removed (removed flag to true), find return
         * past-the-end anyway (see end()).
         *
         * \param key Key value of the element to search for.
         * \return Iterator to the element with key or end() if not found.
         */
        const_iterator find(const Key& key) const {
            const auto elt_it = _map.find(key);
            if(elt_it != _map.end() && !elt_it->second.isRemoved()) {
                const_iterator it(*this);
                it._it = elt_it;
                return it;
            }
            else {
                return this->end();
            }
        }

        /**
         * Count the number of element with this key.
         * Since no duplicate are allowed, return 0 or 1.
         *
         * \param key Key value of the element to count.
         * \return Number of elements with this key, either 0 or 1.
         */
        size_type count(const Key& key) const {
            return (this->find(key) != this->end()) ? 1 : 0;
        }


    // -------------------------------------------------------------------------
    // Modifiers methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Inserts new key in the container.
         * If key already exists, use timestamps for concurrency control.
         *
         * \par Concurrent add / add
         * Timestamp is updated with the higher value. Key is added in any case.
         *
         * \par Concurrent add / remove
         * Uses the higher timestamp select the winning operation.
         * If remove timestamp wins, this add operation does nothing.
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
            U keyStamp      = keyElt.timestamp();

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
         * received before add. (Note that receiving the actual add wont do
         * anything since its timestamps will be smaller).
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
            U keyStamp      = keyElt.timestamp();

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
    // Hash policy methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Change capacity of the container.
         *
         * \see http://en.cppreference.com/w/cpp/container/unordered_map/reserve
         * \param count New capacity of the container.
         */
        void reserve(size_type count) {
            _map.reserve(count);
        }


    // -------------------------------------------------------------------------
    // CRDT Specific
    // -------------------------------------------------------------------------

    public:

        /**
         * Get the actual internal size of the container.
         * This also count elements with removed flag.
         * crdt_size() >= size()
         *
         * \return Internal size of the container.
         */
        float crdt_size() const {
            return _map.size();
        }

        /**
         * Check if tow containers have the exact same internal data.
         * Element with removed flag are used for this comparison.
         *
         * \param other Container to compare with.
         * \return True if equals, otherwise, return false.
         */
        bool crdt_equal(const LWWSet& other) const {
            return (_map == other._map);
        }


    // -------------------------------------------------------------------------
    // Iterators
    // -------------------------------------------------------------------------

    public:

        /**
         * Returns a constant iterator to the beginning.
         *
         * \return Constant iterator to the first element.
         */
        const_iterator begin() const noexcept {
            return const_iterator(*this);
        }

        /**
         * Returns a constant iterator to the end.
         *
         * \return Constant iterator to the last element.
         */
        const_iterator end() const noexcept {
            const_iterator it(*this);
            it._it = _map.end();
            return it;
        }

        /**
         * Returns a constant crdt iterator to the beginning.
         *
         * \see LWWSet::crdt_iterator
         * \return CRDT iterator to the first element.
         */
        const_crdt_iterator crdt_begin() const noexcept {
            return _map.begin();
        }

        /**
         * Returns a constant crdt iterator to the end.
         *
         * \see LWWSet::crdt_iterator
         * \return CRDT iterator to the last element.
         */
        const_crdt_iterator crdt_end() const noexcept {
            return _map.end();
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if lhs and rhs are equals.
         * Two sets are equal if their 'living' set of keys are equal.
         *
         * \warning
         * Only keys are considered. Content of key may differs.
         *
         * \param lhs Left hand side
         * \param rhs Right hand side
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
            for(const auto& elt : lhs) {
                if(rhs.find(elt) == rhs.end()){
                    return false;
                }
            }
            for(const auto& elt: rhs) {
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
         * \see LWWSet::operator==
         *
         * \param lhs Left hand side
         * \param rhs Right hand side
         * \return True if not equal, otherwise, return false.
         */
        friend bool operator!=(const LWWSet& lhs, const LWWSet& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Display the internal content.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out,
                                        const LWWSet<Key,U>& o) {
            out << "CmRDT::LWWSet = ";
            for(const auto& elt : o._map) {
                out << "(" << elt.first
                    << "," << elt.second.timestamp();
                if(elt.second.isRemoved()) {
                    out << ",x) ";
                }
                else {
                    out << ",o) ";
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
 * \brief
 * Represents the CRDT internal state of a key in LWWSet.
 *
 * From outside, LWWSet is seen as std::unordered_set. However, internally,
 * CRDT metadata are required. This class store these CRDT metadata.
 *
 * \par
 * Keys are never removed, only marked as removed with a timestamps.
 * For further information, read about CRDTs (I put some resources in the
 * README)
 *
 *
 * \tparam Key  Type of set elements.
 * \tparam U    Type of timestamps.
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename U>
class LWWSet<Key,U>::Metadata {
    private:
        friend LWWSet;

        U    _timestamp = {0};
        bool _isRemoved = false;

    public:

        /**
         * Returns the current timestamp associated with the key.
         *
         * \return Key's timestamp.
         */
        const U& timestamp() const {
            return _timestamp;
        }

        /**
         * Check whether this key is marked as removed.
         *
         * \return True if removed, otherwise, return false.
         */
        bool isRemoved() const {
            return _isRemoved;
        }

    public:

        friend bool operator==(const Metadata& rhs, const Metadata& lhs) {
            return (rhs._timestamp == lhs._timestamp)
                && (rhs._isRemoved == lhs._isRemoved);
        }

        friend bool operator!=(const Metadata& rhs, const Metadata& lhs) {
            return !(rhs == lhs);
        }
};


/**
 * \brief
 * Constant iterator for LWWSet container.
 *
 * Iterate over all keys that are in this set and are NOT marked as removed.
 * This behave like a normal set iterator.
 *
 *
 * \tparam Key  Type of set elements.
 * \tparam U    Type of timestamps.
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename Key, typename U>
class LWWSet<Key,U>::const_iterator
: public std::iterator<std::input_iterator_tag, Key> {
    private:
        friend LWWSet;

        const LWWSet&       _data;
        const_crdt_iterator _it;


    public:

        const_iterator(const LWWSet& set) : _data(set) {
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

        const Key& operator*() const {
            return _it->first;
        }
};


}} // End namespaces


