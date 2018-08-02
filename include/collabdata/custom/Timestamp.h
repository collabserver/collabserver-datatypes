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
    private:
        std::chrono::steady_clock::time_point _time;
        int _id = 0;
        static int _effectiveID;


    // -------------------------------------------------------------------------
    // Intitialization
    // -------------------------------------------------------------------------

    public:

        /**
         * Create a timestamp with the minimal possible value.
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
         * Returns a timestamps that correspond to current time.
         * This is the most common way to use timestamp class.
         * Only call "Timestamp t = Timestamp::now();" whenever you need it.
         */
        static Timestamp now();

        /**
         * Set the current effective ID.
         * See the paragraph about effective ID (In Timestamp doc) to
         * understand its usage.
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

        friend bool operator==(const Timestamp& rhs, const Timestamp& lhs);
        friend bool operator!=(const Timestamp& rhs, const Timestamp& lhs);
        friend bool operator<(const Timestamp& rhs, const Timestamp& lhs);
        friend bool operator>(const Timestamp& rhs, const Timestamp& lhs);
};


} // End namespace


