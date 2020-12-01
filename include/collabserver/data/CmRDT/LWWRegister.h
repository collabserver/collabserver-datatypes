#pragma once

#include <ostream>

namespace collabserver {
namespace CmRDT {

/**
 * \brief
 * Last-Writer-Wins Register (LWW Register).
 * CmRDT (Operation-based).
 *
 * Register holds any kind of atomic data inside.
 * Timestamps is assigned to each update and creates a total order of updates.
 * Whenever register is actually updated, the old value is totally lost and
 * changed by its new value and timestamp is updated.
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
 * T template parameter must have a default constructor.
 * U timestamp must accept "U t = {0}" (This should set with minimal value).
 *
 *
 * \tparam T    Type of element (Register content).
 * \tparam U    Type of timestamps (Must implements operators > and <).
 */
template <typename T, typename U>
class LWWRegister {
   private:
    T _reg;              // DevNote: We should create init constructor?
    U _timestamp = {0};  // DevNote: this may create some trouble?

    // -------------------------------------------------------------------------
    // Query methods
    // -------------------------------------------------------------------------

   public:
    /**
     * Get a constant reference of the current register value.
     *
     * \return Constant reference of the register content.
     */
    const T& query() const { return _reg; }

    /**
     * Get the internal current timestamps associated with this register.
     *
     * \return Current register's timestamps.
     */
    const U& timestamp() const { return _timestamp; }

    // -------------------------------------------------------------------------
    // Modifiers
    // -------------------------------------------------------------------------

   public:
    /**
     * Change the local register value.
     * Do nothing if given stamp is less to the current timestamps.
     *
     * Returns true if update has been applied. For instance, if timestmap
     * was already higher, update is actually not done and value stay as
     * it was before. (CRDT Property)
     *
     * \par Idempotent
     * Duplicate calls with same stamp is idempotent.
     * This is a valid only if value is the same in all calls.
     *
     * \param value New value to place in this register.
     * \param stamp Timestamp of this update.
     * \return True if update applied, otherwise, returns false.
     */
    bool update(const T& value, const U& stamp) {
        if (stamp > _timestamp) {
            _reg = value;
            _timestamp = stamp;
            return true;
        }
        return false;
    }

    // -------------------------------------------------------------------------
    // CRDT Specific
    // -------------------------------------------------------------------------

   public:
    /**
     * Check if two registers have the exact same internal data.
     * (Data and timestamps)
     *
     * \param other Container to compare with.
     * \return True if equals, otherwise, return false.
     */
    bool crdt_equal(const LWWRegister& other) const { return (_reg == other._reg) && (_timestamp == other._timestamp); }

    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

   public:
    /**
     * Check if two registers are equal.
     * Two registers are equal if their data are equal.
     * Timestamps is not used for equality.
     *
     * \param lhs   Left hand side.
     * \param rhs   Right hand side.
     * \return True if equal, otherwise, return false.
     */
    friend bool operator==(const LWWRegister& lhs, const LWWRegister& rhs) { return (lhs._reg == rhs._reg); }

    /**
     * Check if two registers are different.
     * See operator== for equality meaning.
     *
     * \see LWWRegister::operator==
     *
     * \param lhs   Left hand side.
     * \param rhs   Right hand side.
     * \return True if not equal, otherwise, return false.
     */
    friend bool operator!=(const LWWRegister& lhs, const LWWRegister& rhs) { return !(lhs == rhs); }

    /**
     * Displays the register value and its timestamp.
     * This is mainly for debug print purpose.
     */
    friend std::ostream& operator<<(std::ostream& out, const LWWRegister& o) {
        out << "CmRDT::LWWRegister = (T=" << o.query() << ", U=" << o._timestamp << ")";
        return out;
    }
};

}  // namespace CmRDT
}  // namespace collabserver
