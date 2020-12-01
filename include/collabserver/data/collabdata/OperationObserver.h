#pragma once

namespace collabserver {

class Operation;

/**
 * \brief
 * Interface for component that observes Operations from a CollabData.
 *
 * CollabData modification generates an Operation to describe this change.
 * OperationObserver may be notified of this operation. (If registered)
 * A common use is to update a GUI, update a database or send the operation
 * over the network. (For other replicates)
 *
 *
 * \par Handle operation
 *  - Using a switch-case statement
 *  - Using OperationHandler
 *
 * \par Handle Operation (Switch-case)
 * Operations have a unique ID. At this point, you know the received operation
 * are for your specific custom CollabData. You may cast the received
 * operation to its concrete type according to the ID.
 * This strategy has the advantage that you may be interested in only one (or
 * few) operations from the whole set of operation.
 *
 * \par Handle Operation (OperationHandler)
 * This is the recommended approach but requires OperationHandler is available
 * for your custom CollabData.
 * See OperationHandler for the full documentation about this strategy.
 *
 *
 * \see CollabData
 * \see Operation
 * \see OperationHandler
 */
class OperationObserver {
   protected:
    OperationObserver() = default;
    OperationObserver(const OperationObserver& other) = default;
    OperationObserver& operator=(const OperationObserver& other) = default;

   public:
    virtual ~OperationObserver() = default;

   public:
    /**
     * Receives an operation.
     *
     * \param op Reference to the operation.
     */
    virtual void onOperation(const Operation& op) = 0;
};

}  // namespace collabserver
