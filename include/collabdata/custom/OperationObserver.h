#pragma once

namespace collab {


class Operation;


/**
 * \brief
 * Interface for Operation observer.
 *
 * CollabData modifications generate an Operation to describe this change.
 * OperationObserver may be notified of this operation.
 * A common use is to update a GUI, update a database or send the operation
 * over the network.
 *
 * \see Operation
 * \see CollabData
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


} // End namespace


