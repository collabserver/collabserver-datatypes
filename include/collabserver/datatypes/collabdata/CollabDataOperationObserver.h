#pragma once

#include "CollabDataOperation.h"

namespace collabserver {

/**
 * \brief
 * Interface for component that observes CollabDataOperations from a CollabData.
 *
 * CollabData modification generates an CollabDataOperation to describe this change.
 * CollabDataOperationObserver may be notified of this operation. (If registered)
 * A common use is to update a GUI, update a database or send the operation
 * over the network. (For other replicates)
 *
 *
 * \par Handle operation
 *  - Using a switch-case statement
 *  - Using CollabDataOperationHandler
 *
 * \par Handle CollabDataOperation (Switch-case)
 * CollabDataOperations have a unique ID. At this point, you know the received operation
 * are for your specific custom CollabData. You may cast the received
 * operation to its concrete type according to the ID.
 * This strategy has the advantage that you may be interested in only one (or
 * few) operations from the whole set of operation.
 *
 * \par Handle CollabDataOperation (CollabDataOperationHandler)
 * This is the recommended approach but requires CollabDataOperationHandler is available
 * for your custom CollabData.
 * See CollabDataOperationHandler for the full documentation about this strategy.
 *
 *
 * \see CollabData
 * \see CollabDataOperation
 * \see CollabDataOperationHandler
 */
class CollabDataOperationObserver {
   protected:
    CollabDataOperationObserver() = default;
    CollabDataOperationObserver(const CollabDataOperationObserver& other) = default;
    CollabDataOperationObserver& operator=(const CollabDataOperationObserver& other) = default;

   public:
    virtual ~CollabDataOperationObserver() = default;

   public:
    /**
     * Receives an operation.
     *
     * \param op Reference to the operation.
     */
    virtual void onOperation(const CollabDataOperation& op) = 0;
};

}  // namespace collabserver
