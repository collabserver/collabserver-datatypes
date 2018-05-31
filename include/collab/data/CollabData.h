#pragma once

#include "Operation.h"
#include "OperationObserver.h"

#include <vector>
#include <cassert>

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
        std::vector<OperationObserver*> _operationObservers;


    // -------------------------------------------------------------------------
    // Methods
    // -------------------------------------------------------------------------

    public:

        CollabData() {
            _operationObservers.reserve(3); // Usually I have only 1 observer.
        }

        virtual ~CollabData() = default; // Virtual important for polymorphism


    // -------------------------------------------------------------------------
    // Operation Methods (Observers, notifications).
    // -------------------------------------------------------------------------

    public:

        /**
         * Send a local operation to all OperationObservers.
         *
         * Method that modifies data creates an operations that describes this
         * modification. Several components may request to know about these
         * operations, for instance, to broadcast the operation over the
         * network etc...
         *
         * \param Reference to the operation to broadcast.
         */
        void sendOperation(const Operation& op) {
            for(OperationObserver* const obs : _operationObservers) {
                obs->receiveOperation(op);
            }
        }

        /**
         * Registers a OperationObserver in this data.
         * Does nothing if this observer is already registered and returns false.
         * Does nothing if nullptr is given.
         *
         * \param observer The observer to add.
         * \return True if added, otherwise, return false.
         */
        bool addOperationObserver(OperationObserver* observer) {
            assert(observer != nullptr);
            for(OperationObserver* const obs : _operationObservers) {
                if(obs == observer) {
                    return false;
                }
            }
            _operationObservers.push_back(observer);
            return true;
        }

        /**
         * Return the number of operation observer registered in this data.
         *
         * \return Number of registered OperationObserver.
         */
        std::vector<OperationObserver>::size_type sizeOperationObserver() const {
            return _operationObservers.size();
        }

        /**
         * Apply an external operation on this data.
         * Operation may be received from external components (Ex: network).
         *
         * \param Constant reference to the operation to apply.
         */
        virtual void applyOperation(const Operation& op) = 0;
};


} // End namespace


