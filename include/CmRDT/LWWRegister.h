#pragma once

#include <cassert>
#include <ostream>

namespace CRDT {
namespace CmRDT {


/**
 * Last-Writer-Wins Register (LWW Register).
 * CmRDT (Operation-based).
 *
 * Timestamps is assigned to each update and creates a total order of updates.
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
 *  A Last-Writer-Wins Register (LWW-Register) creates a total order of
 *  assignments by associating a timestamp with each update.
 *  Timestamps are assumed unique, totally ordered, and consistent with causal
 *  order; i.e., if assignment 1 happened-before assignment 2,
 *  the former's timestamp is less than the latter's. This may be implemented as
 *  a per-replicate counter concatenated with a unique replica identifier,
 *  such as its MAC address.
 * "
 *
 *
 * \tparam T    Type of element (Register content).
 * \tparam U    Type of timestamps (Implements operators > and <).
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
template<typename T, typename U>
class LWWRegister {
    private:
        T   _reg;
        U   _timestamp = 0;


    // -------------------------------------------------------------------------
    // CRDT methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Get a constant reference of the current register value.
         *
         * \return Constant reference of the register content.
         */
        const T& query() const {
            return _reg;
        }

        /**
         * Change the local register value. (Downstream update).
         * Do nothing if given stamp is less to the current timestamps.
         *
         * \warning
         * timestamp must be strictly unique. Two equal timestamps is undefined.
         *
         * \param value New value for this register.
         * \param stamp Associated timestamp
         */
        void update(const T& value, const U& stamp) {
            assert(stamp != _timestamp);

            if(stamp > _timestamp) {
                _reg = value;
                _timestamp = stamp;
            }
        }

        /**
         * Get the internal current timestamps.
         *
         * \return Current timestamps associated with this register.
         */
        const U& timestamp() const {
            return _timestamp;
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * Check if two registers are equal.
         * Two registers are equal if their data are equal.
         * Timestamps is not used for equality.
         *
         * \return True if equal, otherwise, return false.
         */
        friend bool operator==(const LWWRegister& lhs, const LWWRegister& rhs) {
            return (lhs._reg == rhs._reg);
        }

        /**
         * Check if two registers are different.
         * See operator== for equality meaning.
         *
         * \return True if not equal, otherwise, return false.
         */
        friend bool operator!=(const LWWRegister& lhs, const LWWRegister& rhs) {
            return !(lhs == rhs);
        }

        /**
         * Displays the register value and its timestamp.
         * This is mainly for debug print purpose.
         */
        friend std::ostream& operator<<(std::ostream& out, const LWWRegister& o) {
            out << "CmRDT::LWWRegister = (T="
                << o.query()
                << ", U=" << o._timestamp
                << ")";
            return out;
        }
};


}} // End namespaces


