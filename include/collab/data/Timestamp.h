#pragma once

#include <chrono>
#include <cassert>

namespace collab {


/**
 * \brief
 * Timestamps implementation for CollabServer.
 *
 * Timestamps should be strictly unique. This implementation mix time value
 * (chrono clock) with a unique integer. (ex: user counter).
 *
 * \author  Constantin Masson
 * \date    Nov 2018
 */
class Timestamp {
    private:
        std::chrono::steady_clock::time_point _time;
        int _uniqueID = 0;

    public:
        static Timestamp now() {
            Timestamp t = 0;
            t._uniqueID = 0; // TODO (ex: get hash of MAC addr)
            t._time = std::chrono::steady_clock::now();
            return t;
        }

        Timestamp(const int value) {
            // Dev note:
            // This may sounds weird, but this is because the only need of
            // U time = x is to set the timestamp to its lower value.
            _uniqueID = 0;
            _time = std::chrono::time_point<std::chrono::steady_clock>::min();
        }


    // -------------------------------------------------------------------------
    // Operators overload
    // -------------------------------------------------------------------------

    public:

        Timestamp& operator=(const int value) {
            // Dev note: see constructor note
            _uniqueID = 0;
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


