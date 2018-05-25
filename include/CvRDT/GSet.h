#pragma once

#include <set>
#include <iterator>
#include <iostream>

namespace CRDT {
namespace CvRDT {


/**
 * \deprecated
 * I originally made this class as an example and is not fully tested.
 * Several update / bug fixing may be required to use it in real world.
 *
 *
 * Grow-only (add-only) Set.
 * CvRDT (State-based).
 *
 * Do not support remove element.
 *
 * \note
 * Internally, uses std::set
 *
 *
 * \tparam T Type of element.
 *
 * \author  Constantin Masson
 * \date    March 2018
 */
template<typename T>
class GSet {
    private:
        std::set<T> _add; // Set of added elements

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
         *
         * \return Size of the set.
         */
        size_type size() const {
            return _add.size();
        }


    // -------------------------------------------------------------------------
    // Modifier
    // -------------------------------------------------------------------------

    public:

        /**
         * Insert a value.
         * Set has no duplicate. Do nothing if already in the set.
         *
         * \param Element value to insert.
         */
        void insert(const T& value) {
            _add.insert(value);
        }

        /**
         * Merge other CRDT with this current data.
         *
         * \param other CRDT to merge with.
         */
        void merge(const GSet<T>& other) {
            for(const T& value : other._add) {
                _add.insert(value);
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
         * Returns a constant iterator to the first element of the container.
         * If the container is empty, the returned iterator will be
         * equal to end().
         *
         * \return Constant iterator to the first element.
         */
        const_iterator cbegin() const {
            return _add.cbegin();
        }

        /**
         * Returns a constant iterator to the element following the last element of
         * the container. This element acts as a placeholder.
         * Attempting to access it results in undefined behavior.
         *
         * \return Constant iterator to the element following the last element.
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
         * Two GSet are equal if their internal add set are equal.
         *
         * \return True if equal, otherwise, return false.
         */
        bool operator==(const GSet& lhs) const {
            return this->_add == lhs._add;
        }

        /**
         * Check if lhs and rhs are not equals.
         * See operator == for further information about equality meaning.
         *
         * \return True if not equal, otherwise, return false.
         */
        bool operator!=(const GSet& lhs) const {
            return !(*this == lhs);
        }

        /**
         * Display the GSet content.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out, const GSet<T>& o) {
            out << "GSet = {";
            for(const T& value : o._add) {
                out << value << " ";
            }
            out << "}";
            return out;
        }
};


}} // End namespaces



