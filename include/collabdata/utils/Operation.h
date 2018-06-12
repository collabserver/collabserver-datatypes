#pragma once

#include <sstream>

namespace collab {


/**
 * \brief
 * Interface that represents any operations on CollabData.
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
class Operation {
    protected:
        int _type = 0; // Unset type
        Operation() = default;
    public:
        virtual ~Operation() = default;

    public:

        /**
         * Returns the type of this operation.
         *
         * \return Operation's type of 0 if unkown type.
         */
        int getType() const {
            return _type;
        }

    public:

        /**
         * Serialize the operation data in internal packed format.
         *
         * \param buffer Where to place serialized data.
         * \return True if successfully serialized, otherwise, return false.
         */
        virtual bool serialize(std::stringstream& buffer) const = 0;

        /**
         *
         * Unserialize the operation from its internal packed format.
         *
         * \param buffer Where to place unserialized data.
         * \return True if successfully unserialized, otherwise, return false.
         */
        virtual bool unserialize(std::stringstream& buffer) = 0;
};


} // End namespace


