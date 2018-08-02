#pragma once

#include <cassert>
#include <chrono>

namespace collab {


/**
 * \brief
 * Timestamps implementation for CollabServer.
 *
 * Timestamps should be strictly unique. This implementation mix time value
 * (chrono clock) with an integer value unique for each user.
 *
 * \par Effective ID
 * Each user must have a unique ID to mix with the chrono value so that,
 * two changes at the exact same time uses the ID to pick up a winner.
 * Whenever a timestamp is created, its 'user id' is set with the
 * current effectiveID.
 */
class Timestamp {
    public:
        typedef std::chrono::steady_clock       Clock;
        typedef std::chrono::time_point<Clock>  TimePoint;

    private:
        TimePoint _time;
        int _id = 0;
        static int _effectiveID;


    // -------------------------------------------------------------------------
    // Intitialization
    // -------------------------------------------------------------------------

    public:

        /**
         * Create a timestamp with the minimal possible value.
         * Uses current effectiveID as timestamp id.
         *
         * \note
         * The parameters is only used to allow "Timestamp t = 0".
         * This may be weird, but this is required by CRDTs.
         * (See implementation). Note that, I had an issue with gcc 4.6.4 and
         * "Timestamp t = 0" was not compiling. Use "Timestamp t = {0}" instead.
         *
         * \param value Dummy value (See note)
         */
        Timestamp(const int value);

        /**
         * Create a timestamp with a specific time.
         * Uses current effectiveID as timestamp id.
         *
         * \param time Time to set in this timestamp.
         */
        explicit Timestamp(const TimePoint time);

        /**
         * Creates a timestamps that correspond to current time.
         * Uses current effectiveID as timestamp id.
         */
        static Timestamp now();

        /**
         * Set the current effective ID.
         * See the paragraph about effective ID (In Timestamp doc) to
         * understand its usage.
         *
         * \see Timestamp
         */
        static void effectiveID(const int id);


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        /**
         * \copydoc Timestamp::Timestamp(const int)
         */
        Timestamp& operator=(const int value);
        Timestamp& operator=(const Timestamp& other);

        friend bool operator==(const Timestamp& rhs, const Timestamp& lhs);
        friend bool operator!=(const Timestamp& rhs, const Timestamp& lhs);
        friend bool operator<(const Timestamp& rhs, const Timestamp& lhs);
        friend bool operator>(const Timestamp& rhs, const Timestamp& lhs);


    // -------------------------------------------------------------------------
    // Getter - Setters
    // -------------------------------------------------------------------------

    public:

        const TimePoint& getTime() const {
            return _time;
        }

        int getID() const {
            return _id;
        }
};


} // End namespace


