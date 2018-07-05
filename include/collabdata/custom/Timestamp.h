#pragma once

#include <chrono>
#include <cassert>

namespace collab {


/**
 * \brief
 * Timestamps implementation for CollabServer.
 *
 * Timestamps should be strictly unique. This implementation mix time value
 * (chrono clock) with an integer value unique for each user.
 *
 * \bug
 * Unique ID is hard coded for now.
 * Should be unique per user. For instance hash of MAC address.
 */
class Timestamp {
    private:
        std::chrono::steady_clock::time_point _time;
        int _uniqueID = 0;
        static const int _localUniqueID = 0; // TODO Unique per user.


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
        Timestamp(const int value) {
            _uniqueID = Timestamp::_localUniqueID;
            _time = std::chrono::time_point<std::chrono::steady_clock>::min();
        }

        /**
         * Returns a timestamps that correspond to current time.
         * This is the most common way to use timestamp class.
         * Only call "Timestamp t = Timestamp::now();" whenever you need it.
         */
        static Timestamp now() {
            Timestamp t     = {0}; // See constructor doc to understand {0}
            t._uniqueID     = Timestamp::_localUniqueID;
            t._time         = std::chrono::steady_clock::now();
            return t;
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        Timestamp& operator=(const int value) {
            // Dev note: see constructor note
            _uniqueID = Timestamp::_localUniqueID;
            _time = std::chrono::time_point<std::chrono::steady_clock>::min();
            return *this;
        }

        friend bool operator==(const Timestamp& rhs, const Timestamp& lhs) {
            return (rhs._time == lhs._time && rhs._uniqueID == lhs._uniqueID);
        }

        friend bool operator!=(const Timestamp& rhs, const Timestamp& lhs) {
            return !(rhs == lhs);
        }

        friend bool operator>(const Timestamp& rhs, const Timestamp& lhs) {
            if(rhs._time == lhs._time) {
                assert(rhs._uniqueID != lhs._uniqueID);
                return (rhs._uniqueID > lhs._uniqueID);
            }
            return (rhs._time > lhs._time);
        }

        friend bool operator<(const Timestamp& rhs, const Timestamp& lhs) {
            if(rhs._time == lhs._time) {
                assert(rhs._uniqueID != lhs._uniqueID);
                return (rhs._uniqueID < lhs._uniqueID);
            }
            return (rhs._time < lhs._time);
        }
};


} // End namespace


