#pragma once

#include <set>
#include <iterator>
#include <iostream>

namespace collab {
namespace CvRDT {


/**
 * \deprecated
 * I originally made this class as an example and is not fully tested.
 * Several update / bug fixing may be required to use it in real world.
 *
 *
 * Two-Phase Set (2P-Set).
 * CvRDT (State-based).
 *
 * Grow Only Add / Remove Set.
 * Elements are added only once and removed only if already added.
 * Once removed, an element cannot be added again.
 * Removing an element is allowed only if already in the 'add' set.
 *
 * \note
 * 2P-Set gives precedences to remove operation.
 * (OR-Set however, gives precedences to add).
 *
 * \note
 * Internally, uses two std::set (Add set and Remove set).
 *
 *
 * \tparam T Type of element.
 */
template<typename T>
class TwoPSet {
    private:
        std::set<T> _add; // Set of added elt (Without removed elt)
        std::set<T> _rem; // Set of removed elt (tombstone)

    public:
        typedef typename std::set<T>::const_iterator        const_iterator;
        typedef typename std::set<T>::size_type             size_type;


    // -------------------------------------------------------------------------
    // Capacity
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if the container has no elements.
         *
         * \return True if empty, otherwise, return false.
         */
        bool empty() const {
            return _add.empty();
        }

        /**
         * Returns the number of elements in the container.
         * This internally count elements in the add set.
         *
         * \return Size of the set.
         */
        size_type size() const {
            return _add.size();
        }


    // -------------------------------------------------------------------------
    // Modifiers
    // -------------------------------------------------------------------------

    public:

        /**
         * Remove all elements from the container.
         * All removed element are added to the tombstone and cannot be
         * added again.
         */
        void clear() {
            for(const T& value : _add) {
                _rem.insert(value);
            }
            _add.clear();
        }

        /**
         * Insert a value.
         * Set has no duplicate. Do nothing if already in the set.
         * Element may be added only once.
         * Do nothing if element has already been removed (Is in tombstone set).
         *
         * \param Element value to insert.
         */
        void insert(const T& value) {
            // Add only if not already removed.
            if(_rem.count(value) == 0) {
                _add.insert(value);
            }
        }

        /**
         * Removes the element with the key equivalent to key.
         *
         * \warning
         * insert and erase are commutative (CRDT Property).
         * Calling erase() before insert() adds the element to the tombstone.
         * It won't be possible to add this element later.
         * (precedence to remove)
         *
         * \param key Key value of the elements to remove.
         * \return Number of elements actually removed.
         */
        size_type erase(const T& key) {
            _rem.insert(key);
            return _add.erase(key);
        }

        /**
         * Merge other CRDT with this current data.
         *
         * \param other CRDT to merge with.
         */
        void merge(const TwoPSet<T>& other) {
            for(const T& value : other._rem) {
                _rem.insert(value);
                if(_add.count(value) != 0) {
                    _add.erase(value);
                }
            }
            for(const T& value : other._add) {
                if(_rem.count(value) == 0) {
                    _add.insert(value);
                }
            }
        }


    // -------------------------------------------------------------------------
    // Lookup
    // -------------------------------------------------------------------------
    public:

        /**
         * Returns the number of elements matching specific key.
         * Since keys are unique, this returns 0 or 1.
         *
         * \note
         * Internally returns the number of elements in add set.
         *
         * \return Number of element with key key.
         */
        size_type count(const T& key) const {
            return _add.count(key);
        }

        /**
         * Finds an element with key equivalent to key.
         *
         * \param key key value of the element to search for.
         * \return Constant iterator to the element (Or end if no element).
         */
        const_iterator find(const T& key) const {
            return _add.find(key);
        }


    // -------------------------------------------------------------------------
    // Iterators
    // -------------------------------------------------------------------------

    public:

        /**
         * Returns a const iterator to the first element of the container.
         * If the container is empty, the returned iterator will be
         * equal to end().
         *
         * \return Const iterator to the first element.
         */
        const_iterator cbegin() const {
            return _add.cbegin();
        }

        /**
         * Returns a const iterator to the element following the last element of
         * the container. This element acts as a placeholder.
         * Attempting to access it results in undefined behavior.
         *
         * \return Const iterator to the element following the last element.
         */
        const_iterator cend() const {
            return _add.cend();
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------
    public:

        /**
         * Check if lhs and rhs are equals.
         * Two 2PSet lists are equal if and only if they internal add set are
         * equals and their internal remove set are equals.
         *
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const TwoPSet& lhs, const TwoPSet& rhs) {
            return (lhs._add == rhs._add) && (lhs._rem == rhs._rem);
        }

        /**
         * Check if lhs and rhs are not equals.
         * See operator == for further information about equality meaning.
         *
         * \return True if not equal, otherwise, return false.
         */
        friend bool operator!=(const TwoPSet& lhs, const TwoPSet& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Display the actual 2PSet's internal state.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out, const TwoPSet& o) {
            out << "2PSet = add {";
            for(const T& value : o._add) {
                out << value << " ";
            }
            out << "} - rem {";
            for(const T& value : o._rem) {
                out << value << " ";
            }
            out << "}";
            return out;
        }
};


}} // End namespace


