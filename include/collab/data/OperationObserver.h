#pragma once

class Operation;

namespace collab {


/**
 * \brief
 * Interface for Operation observer.
 *
 * Whenever a CollabData is modified, an operation is created.
 * This operation is broadcasted to all OperationObserver.
 * For instance, a server may listen to operation in order to send then over
 * the network whenever local data is modified (And then observer notified).
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
class OperationObserver {

    public:

        /**
         * Receive an operation from a CollabData.
         *
         * \warning
         * If you need to keep track of this operation (ex: vector of operations)
         * do a copy of this operation since the reference is local to the caller
         * method and is deleted soon.
         *
         * \param op Reference the operation received.
         */
        void receiveOperation(const Operation& op);
};


} // End namespace
