#pragma once

#include <ostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>  // std::pair

namespace collabserver {

/**
 * \brief
 * Last-Writer-Wins Map (LWW Map).
 * CmRDT (Operation-based)
 *
 * Associative container that contains key-value pairs with unique keys.
 * Internally uses std::unordered_map to store the pair and its CRDT metadata.
 * As the end user, you see this container as a std::unordered_map (See iterator
 * for instance). You may request the actual internal data using crdt_iterator.
 * Check out std::unordered_map documentation for further informations.
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
 * CRDT Map only deals with concurrent add / remove of keys!!
 * By default, key's content is NOT CRDT! This means, several user working
 * concurrently eventually end up with the same map of keys but may have
 * different key values. This is because we cannot deal with keys add / remove
 * and key content update at the same place. For instance, you may have a
 * 'map of map of register' and this container don't know how to update its
 * content.
 * To have a map of CRDT atomic content, you may use a map of LWWRegister
 * for instance. To update a key value, use query on this key and call the
 * register update function.
 *
 * \warning
 * Timestamps are strictly unique for each user's operation, with total order.
 * For any distinct operations (ex: add(t1) / remove(t2)), t1==t2 must return
 * false. (See quote and implementation for further informations).
 *
 * \warning
 * T type must have a default constructor.
 * U timestamp must accept "U t = {0}". (This should set the minimal value.)
 *
 * \see http://en.cppreference.com/w/cpp/container/unordered_map
 *
 *
 * \tparam Key  Type of key.
 * \tparam T    Type of element.
 * \tparam U    Type of timestamps (Must implements operators > and <).
 */
template <typename Key, typename T, typename U>
class LWWMap {
   public:
    class Element;
    class iterator;
    class const_iterator;

    typedef typename std::unordered_map<Key, Element>::size_type size_type;
    typedef typename std::unordered_map<Key, Element>::iterator crdt_iterator;
    typedef typename std::unordered_map<Key, Element>::const_iterator const_crdt_iterator;

    // From outside, we see LWWMap as <Key, T> (Except crdt_iterator)
    typedef typename std::unordered_map<Key, T>::key_type key_type;
    typedef typename std::unordered_map<Key, T>::mapped_type mapped_type;
    typedef typename std::unordered_map<Key, T>::value_type value_type;
    typedef typename std::unordered_map<Key, T>::reference reference;
    typedef typename std::unordered_map<Key, T>::const_reference const_reference;
    typedef typename std::unordered_map<Key, T>::pointer pointer;
    typedef typename std::unordered_map<Key, T>::const_pointer const_pointer;

   private:
    std::unordered_map<Key, Element> _map;
    size_type _sizeAlive = 0;  // Nb of alive elts (Not marked as removed)
    U _lastClearTime = {0};    // Last time a clear has been applied

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
    bool empty() const noexcept { return _sizeAlive == 0; }

    /**
     * Check if the container has no elements.
     * This also takes into account 'removed' elements.
     * (Internal CRDT data).
     *
     * \return True if the container is empty, false otherwise.
     */
    bool crdt_empty() const noexcept { return _map.empty(); }

    /**
     * Returns the number of elements in the container.
     * Only elements that are not marked as 'removed' count.
     *
     * \return Number of elements in the container.
     */
    size_type size() const noexcept { return _sizeAlive; }

    /**
     * Get the actual internal size of the container.
     * This also count elements with removed flag.
     * crdt_size() >= size()
     *
     * \return Internal size of the container.
     */
    size_type crdt_size() const { return _map.size(); }

    /**
     * \see http://en.cppreference.com/w/cpp/container/unordered_map/max_size
     *
     * \return Maximum number of elements.
     */
    size_type max_size() const noexcept { return _map.max_size(); }

    // -------------------------------------------------------------------------
    // Lookup methods
    // -------------------------------------------------------------------------

   public:
    /**
     * Returns a reference to the mapped value of the element with key
     * equivalent to key. If no such element exists, an exception of
     * type std::out_of_range is thrown.
     *
     * \param key Key value of the element to search for.
     * \return Reference to the mapped value of the requested element.
     */
    T& at(const Key& key) {
        auto elt_it = _map.find(key);
        if (elt_it == _map.end() || elt_it->second.isRemoved()) {
            throw std::out_of_range("No element for this key");
        }
        return elt_it->second.value();
    }

    /**
     * \copydoc LWWMap::at
     */
    const T& at(const Key& key) const { return this->at(key); }

    /**
     * Returns a reference to the mapped value of the element with key
     * equivalent to key. If no such element exists, an exception of
     * type std::out_of_range is thrown.
     *
     * Also lookup for 'removed' element (Internal CRDT data).
     *
     * \param key Key value of the element to search for.
     * \return Reference to the mapped value of the requested element.
     */
    T& crdt_at(const Key& key) {
        auto elt_it = _map.find(key);
        if (elt_it == _map.end()) {
            throw std::out_of_range("No element for this key");
        }
        return elt_it->second.value();
    }

    /**
     * \copydoc LWWMap::crdt_at
     */
    const T& crdt_at(const Key& key) const { return this->crdt_at(key); }

    /**
     * Find a key-element in the container.
     *
     * This only lookup for key that are not internally deleted.
     * (Like a normal map::find method).
     * If element is internally removed (removed flag is true), find returns
     * past-the-end anyway (see end()).
     *
     * \param key Key value of the element to search for.
     * \return Iterator to the element with key or end() if not found.
     */
    iterator find(const Key& key) {
        const auto elt_it = _map.find(key);
        if (elt_it != _map.end() && !elt_it->second.isRemoved()) {
            iterator it(*this, elt_it);
            return it;
        } else {
            return this->end();
        }
    }

    /**
     * \copydoc LWWMap::find
     */
    const_iterator find(const Key& key) const {
        const auto elt_it = _map.find(key);
        if (elt_it != _map.end() && !elt_it->second.isRemoved()) {
            const_iterator it(*this, elt_it);
            return it;
        } else {
            return this->end();
        }
    }

    /**
     * Query a key-element and its internal CRDT metadata.
     *
     * If element with this key exists in the internal container, it is
     * returned, regardless its 'removed' status.
     * This may be useful for datatypes built on top of this set.
     * To have CRDT updates, you must apply the update, regardless its
     * internal status. This query will return the element in any case
     * (If already added once in the container.)
     *
     * If this key has never been added, returns crdt past-the-end
     * (See crdt_end()) iterator.
     *
     * \param key The key to query.
     * \return Iterator CRDT to the key or crdt_end() if not found.
     */
    crdt_iterator crdt_find(const Key& key) { return _map.find(key); }

    /**
     * \copydoc LWWMap::crdt_find
     */
    const_crdt_iterator crdt_find(const Key& key) const { return _map.find(key); }

    /**
     * Count the number of element with this key.
     * Since no duplicate are allowed, return 0 or 1.
     *
     * \param key Key value of the element to count.
     * \return Number of elements with this key, either 0 or 1.
     */
    size_type count(const Key& key) const { return (this->find(key) != this->end()) ? 1 : 0; }

    /**
     * Count the number of element with this key.
     * Also lookup for 'removed' element (Internal CRDT data).
     *
     * \param key Key value of the element to count.
     * \return Number of elements with this key, either 0 or 1.
     */
    size_type crdt_count(const Key& key) const { return _map.count(key); }

    // -------------------------------------------------------------------------
    // Modifiers methods
    // -------------------------------------------------------------------------

   public:
    /**
     * Removed all elements from the container.
     *
     * \par Idempotent
     * Duplicate calls with same stamp is idempotent.
     *
     * Only elements with timestamp inferior to clear timestamp are
     * actually removed.
     *
     * \warning
     * Container may not be empty after clear call.
     * This is because if a really older clear is applied, this doesn't
     * affect elements that have been added later.
     * From a user point of view, if you display a UI after a clear, you
     * should iterate over the set anyway.
     *
     * \param stamp Timestamp of this operation.
     * \return True if clear actually applied, otherwise, return false.
     */
    bool clear(const U& stamp) noexcept {
        if (stamp > _lastClearTime) {
            _lastClearTime = stamp;

            // DevNote: Same code as 'remove' but without the insert attempt
            for (auto& elt_it : _map) {
                Element& elt = elt_it.second;

                if (stamp > elt._timestamp) {
                    elt._timestamp = stamp;

                    if (elt._isRemoved == false) {
                        elt._isRemoved = true;
                        --_sizeAlive;
                    }
                }
            }
            return true;
        }

        return false;
    }

    /**
     * Inserts new key in the container.
     * If key already exists, use timestamps for concurrency control.
     *
     * \par Concurrent add / add
     * Timestamp is updated with the higher value. Key is added in any case.
     * Returns false. This is because, it only updates timestamp of the
     * operation. Key was already added in container before the operation.
     *
     * \par Concurrent add / remove
     * Uses the higher timestamp select the winning operation.
     * If remove timestamp wins, this add operation does nothing.
     * Otherwise, add is applied and true is returned.
     *
     * \note
     * This only adds the key. A default element is created.
     * To add key and set its content, call this add method and query the
     * added element (Then update it with its default value).
     *
     * \par Idempotent
     * Duplicate calls with same stamp is idempotent.
     *
     * \param key   Key of the element to add.
     * \param stamp Timestamps of this operation.
     * \return True if key added, otherwise, return false.
     */
    bool add(const Key& key, const U& stamp) {
        Element newElt(key);  // Content is not set here
        newElt._timestamp = stamp;
        newElt._isRemoved = false;

        auto coco_it = _map.insert(std::make_pair(key, newElt));
        bool isKeyAdded = coco_it.second;
        Element& elt = coco_it.first->second;
        const U& keyStamp = elt.timestamp();

        if (!isKeyAdded) {
            if (stamp > keyStamp) {
                elt._timestamp = stamp;

                if (elt._isRemoved == true) {
                    elt._isRemoved = false;
                    ++_sizeAlive;
                    return true;
                }
            }
            return false;
        } else {
            if (stamp > _lastClearTime) {
                ++_sizeAlive;
                return true;
            } else {
                elt._timestamp = _lastClearTime;
                elt._isRemoved = true;
                return false;
            }
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
     * If element is removed, returns true. In case element was already
     * removed, returns false (Though timestamps may have been internally
     * updated for CRDT properties). Note that remove may be called before
     * add, this returns false anyway.
     *
     * \par Idempotent
     * Duplicate calls with same stamp is idempotent.
     *
     * \param key   Key of the element to add.
     * \param stamp Timestamps of this operation.
     * \return True if key removed, otherwise, return false.
     */
    bool remove(const Key& key, const U& stamp) {
        Element newElt(key);  // Content is not set here
        newElt._timestamp = stamp;
        newElt._isRemoved = true;

        auto coco_it = _map.insert(std::make_pair(key, newElt));
        bool isKeyAdded = coco_it.second;
        Element& elt = coco_it.first->second;
        const U& keyStamp = elt.timestamp();

        if (!isKeyAdded) {
            if (stamp > keyStamp) {
                elt._timestamp = stamp;

                if (elt._isRemoved == false) {
                    elt._isRemoved = true;
                    --_sizeAlive;
                    return true;
                }
            }
        }
        return false;  // DevNote: see LWWSet::remove
    }

    // -------------------------------------------------------------------------
    // Hash policy methods
    // -------------------------------------------------------------------------

   public:
    /**
     * Change capacity of the container.
     *
     * \see http://en.cppreference.com/w/cpp/container/unordered_map/reserve
     *
     * \param count New capacity of the container.
     */
    void reserve(size_type count) { _map.reserve(count); }

    // -------------------------------------------------------------------------
    // CRDT Specific
    // -------------------------------------------------------------------------

   public:
    /**
     * Check if two containers have the exact same internal data.
     * Element with removed flag are used for this comparison.
     *
     * \bug
     * crdt_equal is not called recursively if the map content itself is
     * a crdt data. The normal operator== is called in this case.
     * This is because at this point, map can't know if its content data is
     * made from a CRDT type or a normal cpp type (such as int, float etc).
     *
     * \param other Container to compare with.
     * \return True if equals, otherwise, return false.
     */
    bool crdt_equal(const LWWMap& other) const { return _map == other._map; }

    // -------------------------------------------------------------------------
    // Iterator
    // -------------------------------------------------------------------------

   public:
    /**
     * Returns an iterator to the beginning.
     *
     * \return iterator to the first element.
     */
    iterator begin() { return iterator(*this); }

    /**
     * \copydoc LWWMap::begin
     */
    const_iterator begin() const noexcept { return const_iterator(*this); }

    /**
     * Returns an iterator to the end.
     *
     * \return iterator to the last element.
     */
    iterator end() {
        iterator it(*this, _map.end());
        return it;
    }

    /**
     * \copydoc LWWMap::end
     */
    const_iterator end() const noexcept {
        const_iterator it(*this, _map.end());
        return it;
    }

    /**
     * \copydoc LWWMap::begin
     */
    const_iterator cbegin() const noexcept { return const_iterator(*this); }

    /**
     * \copydoc LWWMap::end
     */
    const_iterator cend() const noexcept {
        const_iterator it(*this, _map.end());
        return it;
    }

    /**
     * Returns a crdt_iterator to the beginning.
     *
     * \see LWWMap::crdt_iterator
     * \return CRDT iterator to the first element.
     */
    crdt_iterator crdt_begin() noexcept { return _map.begin(); }

    /**
     * \copydoc LWWMap::crdt_begin
     */
    const_crdt_iterator crdt_begin() const noexcept { return _map.cbegin(); }

    /**
     * Returns a crdt_iterator to the end.
     *
     * \see LWWMap::crdt_iterator
     * \return CRDT iterator to the last element.
     */
    crdt_iterator crdt_end() noexcept { return _map.end(); }

    /**
     * \copydoc LWWMap::crdt_end
     */
    const_crdt_iterator crdt_end() const noexcept { return _map.cend(); }

    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

   public:
    /**
     * Check if lhs and rhs are equals.
     * Two LWWMap are equal if their 'living' set of keys are equal.
     *
     * \param lhs Left hand side
     * \param rhs Right hand side
     * \return True if equal, otherwise, return false.
     */
    friend bool operator==(const LWWMap& lhs, const LWWMap& rhs) {
        if (lhs.size() != rhs.size()) {
            return false;
        }

        // Dev note: in the worst case, this is N2 complexity.
        // See note in LWWSet::operator==
        for (const auto& elt : lhs) {
            const_iterator other_it = rhs.find(elt.first);
            ;
            if (other_it == rhs.cend()) {
                return false;
            }
            if (elt.second != other_it->second) {
                return false;
            }
        }
        for (const auto& elt : rhs) {
            const auto other_it = lhs.find(elt.first);
            if (other_it == lhs.cend()) {
                return false;
            }
            if (elt.second != other_it->second) {
                return false;
            }
        }

        return true;
    }

    /**
     * Check if lhs and rhs are not equals.
     * See operator == for further information about equality meaning.
     *
     * \see LWWMap::operator==
     *
     * \param lhs Left hand side
     * \param rhs Right hand side
     * \return True if not equal, otherwise, return false.
     */
    friend bool operator!=(const LWWMap& lhs, const LWWMap& rhs) { return !(lhs == rhs); }

    /**
     * Display the internal content.
     * This is mainly for debug print purpose.
     */
    friend std::ostream& operator<<(std::ostream& out, const LWWMap<Key, T, U>& o) {
        out << "CmRDT::LWWMap = ";
        for (const auto& elt : o._map) {
            out << "\n  (" << elt.first << ", " << elt.second.value() << ", " << elt.second.timestamp();
            if (elt.second.isRemoved()) {
                out << ", x)";
            } else {
                out << ", o)";
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
 * Actual internal content of each key in the LWWMap.
 *
 * From outside, LWWMap is seen as std::unordered_map. However, internally,
 * CRDT metadata are required. This class store the actual user content and
 * all these CRDT metadata.
 *
 * \par
 * Keys are never removed, only marked as removed with a timestamps.
 * For further information, read about CRDTs
 * (I put some resources in the README)
 *
 *
 * \tparam Key  Type of key.
 * \tparam T    Type of element.
 * \tparam U    Type of timestamps.
 */
template <typename Key, typename T, typename U>
class LWWMap<Key, T, U>::Element {
   private:
    friend LWWMap;

    // I did this for the iterator* method
    // This is possibly not the best solution
    // Actual element value is in _internalValue.second (Burk! Ugly!)
    std::pair<const Key, T> _internalValue;

    U _timestamp = {0};
    bool _isRemoved = false;

    // -------------------------------------------------------------------------
    // Initialization
    // -------------------------------------------------------------------------

   public:
    Element(const Key& key) : _internalValue(std::make_pair(key, T{})) {
        // TODO: T must have default constructor.
        // This may be too restrictive for end-user.
        // I should think about another way.
    }

    // -------------------------------------------------------------------------
    // Methods
    // -------------------------------------------------------------------------

   public:
    /**
     * Returns a reference to the Key's value.
     *
     * \return Reference to the value.
     */
    T& value() { return _internalValue.second; }

    /**
     * Returns a constant reference to the key's value.
     *
     * \return Constant reference to the value.
     */
    const T& value() const { return _internalValue.second; }

    /**
     * Returns the current timestamp associated with the key.
     *
     * \return Key's timestamp.
     */
    const U& timestamp() const { return _timestamp; }

    /**
     * Check whether this key is marked as removed.
     *
     * \return True if removed, otherwise, return false.
     */
    bool isRemoved() const { return _isRemoved; }

    // -------------------------------------------------------------------------
    // Operator overload
    // -------------------------------------------------------------------------

   public:
    /**
     * Check whether two Element are internally the same.
     *
     * \warning
     * Though it is the normal operator==, it is meant to be used as
     * crdt_equal (For internal uses).
     */
    friend bool operator==(const Element& rhs, const Element& lhs) {
        // TODO Can we find a way to call crdt_equal on internalValue?
        // See the crdt_equal 'bug' note. But anyway, it is maybe better
        // like this.
        return (rhs._internalValue == lhs._internalValue) && (rhs._isRemoved == lhs._isRemoved) &&
               (rhs._timestamp == lhs._timestamp);
    }
};

/**
 * \brief
 * Iterator for LWWMap container.
 *
 * Iterate over all keys-elements that are in set and are NOT marked as removed.
 *
 *
 * \tparam Key  Type of key.
 * \tparam T    Type of element.
 * \tparam U    Type of timestamps.
 */
template <typename Key, typename T, typename U>
class LWWMap<Key, T, U>::iterator : public std::iterator<std::input_iterator_tag, value_type> {
   private:
    LWWMap& _data;
    crdt_iterator _it;

   public:
    explicit iterator(LWWMap& map) : _data(map) {
        _it = _data._map.begin();

        // If first element is already removed, skip it
        while (_it != _data._map.end() && _it->second.isRemoved()) {
            ++_it;
        }
    }

    explicit iterator(LWWMap& map, crdt_iterator start) : _data(map), _it(start) {
        while (_it != _data._map.end() && _it->second.isRemoved()) {
            ++_it;
        }
    }

   public:
    iterator& operator++() {
        ++_it;

        while (_it != _data._map.end() && _it->second.isRemoved()) {
            ++_it;
        }
        return *this;
    }

    bool operator==(const iterator& other) const { return _it == other._it; }

    bool operator!=(const iterator& other) const { return !(*this == other); }

    reference operator*() const { return _it->second._internalValue; }

    pointer operator->() const { return &(_it->second._internalValue); }
};

/**
 * \brief
 * Constant iterator for LWWMap container.
 *
 * Iterate over all keys-elements that are in set and are NOT marked as removed.
 *
 *
 * \tparam Key  Type of key.
 * \tparam T    Type of element.
 * \tparam U    Type of timestamps.
 */
template <typename Key, typename T, typename U>
class LWWMap<Key, T, U>::const_iterator : public std::iterator<std::input_iterator_tag, value_type> {
   private:
    const LWWMap& _data;
    const_crdt_iterator _it;

   public:
    explicit const_iterator(const LWWMap& map) : _data(map) {
        _it = _data._map.begin();

        // If first element is already removed, skip it
        while (_it != _data._map.end() && _it->second.isRemoved()) {
            ++_it;
        }
    }

    explicit const_iterator(const LWWMap& map, const_crdt_iterator start) : _data(map), _it(start) {
        while (_it != _data._map.end() && _it->second.isRemoved()) {
            ++_it;
        }
    }

   public:
    const_iterator& operator++() {
        ++_it;

        while (_it != _data._map.end() && _it->second.isRemoved()) {
            ++_it;
        }
        return *this;
    }

    bool operator==(const const_iterator& other) const { return _it == other._it; }

    bool operator!=(const const_iterator& other) const { return !(*this == other); }

    const_reference operator*() const { return _it->second._internalValue; }

    const_pointer operator->() const { return &(_it->second._internalValue); }
};

}  // namespace collabserver
