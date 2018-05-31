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
         * \param op Copy of the operation received.
         */
        void receiveOperation(Operation op);
};


} // End namespace
