#pragma once

namespace collab {


class Operation;


/**
 * \brief
 * Interface for Operation observer.
 *
 * Whenever a CollabData is modified, an operation is created.
 * This operation is broadcasted to all OperationObservers.
 * For instance, a server may listen to operation in order to send then over
 * the network whenever local data is modified (And then observer notified).
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
         * Notify observers that an operation was just applied.
         *
         * \warning
         * If you need to keep track of this operation (ex: vector of
         * operations) do a copy of this operation since the reference is local
         * to the caller method and is deleted soon.
         *
         * \param op Reference the operation received.
         */
        virtual void notifyOperation(const Operation& op) = 0;
};


} // End namespace


