#pragma once

#include <cassert>
#include <ostream>

namespace collab {
namespace CvRDT {

/**
 * \deprecated
 * I originally made this class as an example and is not fully tested.
 * Several update / bug fixing may be required to use it in real world.
 *
 *
 * Last-Writer-Wins Register (LWW Register).
 * CvRDT (State-based).
 *
 * Timestamp is assigned to each update and creates a total order of updates.
 *
 * \note
 * Quote from the CRDT article "A comprehensive study of CRDT":
 *
 * "
 * A Last-Writer-Wins Register (LWW-Register) creates a total order of
 * assignments by associating a timestamp with each update.
 * Timestamps are assumed unique, totally ordered, and consistent with causal
 * order; i.e., if assignment 1 happened-before assignment 2,
 * the former's timestamp is less than the latter's. This may be implemented as
 * a per-replicate counter concatenated with a unique replica identifier,
 * such as its MAC address.
 * "
 *
 * \warning
 * Timestamps must have a total order.
 * Two equal timestamps (t1 == t1 returns true) is undefined and replicated
 * may diverge. (See implementation).
 * (Ex: use a mix of datetime + user id for instance).
 *
 *
 * \tparam T    Type of element.
 * \tparam Key  Timestamps key.
 */
template <typename T, typename Key>
class LWWRegister {
   private:
    T _reg;
    Key _timestamp = 0;

   public:
    /**
     * Get a constant reference of the current register value.
     *
     * \return Constant reference of the register content.
     */
    const T& query() const { return _reg; }

    /**
     * Assign new value to the register.
     * Do nothing if given stamp is less or equal to the current timestamps.
     *
     * TODO: create return value.
     *
     * \param value New value for this register.
     * \param stamp Associated timestamp
     */
    void set(const T& value, const Key& stamp) {
        if (stamp > _timestamp) {
            _timestamp = stamp;
            _reg = value;
        }
    }

    /**
     * Merge this register with another.
     * Use Last-Writer-Wins as modification order.
     * (See required timestamp properties)
     */
    void merge(const LWWRegister& other) {
        assert(other._timestamp != _timestamp);

        if (other._timestamp > _timestamp) {
            _reg = other._reg;
            _timestamp = other._timestamp;
        }
    }

    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

   public:
    /**
     * Check if two registers are equal.
     * Register data and timestamp must be equal for registers to be equal.
     *
     * \return True if equal, otherwise, return false.
     */
    friend bool operator==(const LWWRegister& lhs, const LWWRegister& rhs) {
        return (lhs._reg == rhs._reg) && (lhs._timestamp == rhs._timestamp);
    }

    /**
     * Check if two registers are different.
     * See operator== for equality meaning.
     *
     * \return True if not equal, otherwise, return false.
     */
    friend bool operator!=(const LWWRegister& lhs, const LWWRegister& rhs) { return !(lhs == rhs); }

    /**
     * Display the register value.
     * This is mainly for debug print purpose.
     */
    friend std::ostream& operator<<(std::ostream& out, const LWWRegister& o) {
        out << "CvRDT::LWWRegister: " << o.query();
        return out;
    }
};

}  // namespace CvRDT
}  // namespace collab
