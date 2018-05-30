#pragma once


class CollabInstance;

namespace collab {


/**
 * \brief
 * Data structure CollabServer is working with.
 *
 * This is the high level wrapper of any data CollabServer is working with.
 * Any data used by CollabServer implements this interface. To be fullay CRDT,
 * this data is only composed of CRDTs (From collab/CmRDT for instance).
 *
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
class CollabData {
    private:
        CollabInstance* _collabInstance = nullptr;

    public:
        CollabData() = default;
        virtual ~CollabData() = default;
};


} // End namespace


