#pragma once

#include <sstream>

#include "CollabDataOperationHandler.h"

namespace collabserver {

/**
 * \brief
 * Interface of any operation applicable on a CollabData.
 *
 * Each operation on a CollabData has a unique ID for this data.
 *
 * \see CollabData
 * \see CollabDataOperationObserver
 * \see CollabDataOperationHandler
 */
class CollabDataOperation {
   protected:
    CollabDataOperation() = default;
    CollabDataOperation(const CollabDataOperation& other) = default;
    CollabDataOperation& operator=(const CollabDataOperation& other) = default;

   public:
    virtual ~CollabDataOperation() = default;

   public:
    /**
     * Get the type of this operation.
     * Returns 0 if not type set. (This may be an internal error.)
     *
     * \return ID of the operation's type.
     */
    virtual unsigned int getType() const = 0;

    /**
     * Serialize the operation data in internal packed format.
     *
     * \param buffer Where to place serialized data.
     * \return True if successfully serialized, otherwise, return false.
     */
    virtual bool serialize(std::stringstream& buffer) const = 0;

    /**
     * Unserialize the operation from its internal packed format.
     *
     * \param buffer Where to place unserialized data.
     * \return True if successfully unserialized, otherwise, return false.
     */
    virtual bool unserialize(const std::stringstream& buffer) = 0;

    /**
     * Apply an handler on this operation.
     * This is based on visitor pattern.
     *
     * \param handler The famous concrete handler to use.
     */
    virtual void accept(CollabDataOperationHandler& handler) const = 0;
};

}  // namespace collabserver
