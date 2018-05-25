#pragma once

#include "GCounter.h"

#include <iostream>

namespace CRDT {
namespace CvRDT {


/**
 * \deprecated
 * I originally made this class as an example and is not fully tested.
 * Several update / bug fixing may be required to use it in real world.
 *
 *
 * Increment / Decrement Counter.
 * CvRDT (State-based).
 *
 * \note
 * Internally, uses two Grow Only counters. (Positive / Negative).
 *
 * \warning
 * Data type T must work with std::max. (is_arithmetic == true)
 *
 *
 * \tparam T Type of counter. (Default int)
 * \tparam ID Type of the id used to identify each replicated. (Each unique).
 *
 * \author  Constantin Masson
 * \date    March 2018
 */
template <typename T = int, typename Key = std::string>
class PNCounter {
    private:
        GCounter<T, Key> _positive; // Positive counter
        GCounter<T, Key> _negative; // Negative counter

    public:
        PNCounter(const Key id) : _positive(id), _negative(id) {}


    // -------------------------------------------------------------------------
    // Methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Query a copy of the counter value.
         */
        T query() const {
            return _positive.query() - _negative.query();
        }

        /**
         * Increment counter by one.
         */
        void increment() {
            _positive.increment();
        }

        /**
         * Increment counter by specific amount.
         *
         * \warning
         * Positive number expected.
         * See GSet doc for further information.
         *
         * \see GSet::increment(const T& value)
         */
        void increment(const T& value) {
            _positive.increment(value);
        }

        /**
         * Decrement counter by one.
         */
        void decrement() {
            _negative.increment();
        }

        /**
         * Increment counter by specific amount.
         *
         * \warning
         * Positive number expected.
         * See GSet doc for further information.
         *
         * \see GSet::increment(const T& value)
         */
        void decrement(const T& value) {
            _negative.increment(value);
        }

        /**
         * Merge with another counter.
         */
        void merge(const PNCounter& other) {
            _positive.merge(other._positive);
            _negative.merge(other._negative);
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if lhs and rhs are equals.
         * Two PNCounter are equal if their internal state are equals.
         * (Positive Set and Negative Set equal).
         *
         * \return True if strictly equal, otherwise, return false.
         */
        friend bool operator==(const PNCounter& lhs, const PNCounter& rhs) {
            return (lhs._positive == rhs._positive)
                && (lhs._negative == rhs._negative);
        }

        /**
         * Check if lhs and rhs are not equals.
         * See operator == for further information about equality meaning.
         *
         * \return True if strictly not equal, otherwise, return false.
         */
        friend bool operator!=(const PNCounter& lhs, const PNCounter& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Display internal content.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out, const PNCounter& o) {
            out << "PNCounter = " << o.query();
            out << " (" << o._positive << "), (" << o._negative << ")";
            return out;
        }
};


}} // End namespaces


