#pragma once

#include <chrono>

namespace collab {


/**
 * \brief
 * Timestamps implementation for CollabServer.
 *
 * Timestamps should be strictly unique. This implementation mix time value
 * (chrono clock) with an integer value unique for each user.
 *
 * \par unique IDs
 * Each user must have a unique ID mixed with the timestamp (So that similar
 * timestamp are resolved using the ID).
 *
 * \par Effective ID
 * Whenever a timestamp is created, its 'user ID' is set with the current
 * effectiveID.
 *
 */
class Timestamp {
    public:
        typedef std::chrono::steady_clock       Clock;
        typedef std::chrono::time_point<Clock>  TimePoint;

    private:
        TimePoint           _time;
        unsigned int        _id = 0;
        static unsigned int _effectiveID;


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
         * Create a timestamp with a specific time and id.
         *
         * \param time Time to set in this Timestamp.
         * \param id User ID to set.
         */
        explicit Timestamp(const TimePoint time, const int id);

        /**
         * Creates a timestamps that correspond to current time.
         * Uses current effectiveID as ID.
         *
         * \return Timestamp corresponding to current time.
         */
        static Timestamp now();

        /**
         * Set the current effective ID.
         * See the paragraph about effective ID (In Timestamp doc) to
         * understand its usage.
         *
         * \param id Current effective ID to use.
         * \see Timestamp
         */
        static void setEffectiveID(const unsigned int id);


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

        const TimePoint& getTime() const { return _time; }

        /**
         * Get the actual Timestamp's ID.
         * This is not the effectiveID but the actual ID for this specific
         * Timestamp instance.
         */
        unsigned int getID() const { return _id; }
};


} // End namespace


