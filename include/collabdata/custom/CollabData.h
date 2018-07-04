#pragma once

#include <vector>
#include <cassert>
#include <sstream>

#include "Operation.h"
#include "OperationObserver.h"

namespace collab {


/**
 * \brief
 * Abstract class for high level data built on top of CRDTs.
 *
 * This is the high level interface of any data CollabServer is working with.
 * Any data used by CollabServer implements this interface. To be fully CRDT,
 * this data is only composed of CRDTs (From collab/CmRDT for instance).
 *
 * \par Operation
 * Any modification on the data creates an Operation that describe this change.
 * This may be useful for component using this data to update the display,
 * for instance, in case of GUI, or broadcast the operation in case of network.
 * To receive operation, you must register your component as an Observer of
 * this data.
 *
 * \par Custom data
 * To build your application data on top of the implemented CRDTs such as
 * LWWGraph, LWWMap etc, you may extend this class. This gives you the required
 * methods.
 *
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
class CollabData {
    private:
        std::vector<OperationObserver*> _operationObservers;
        OperationObserver* _broadcaster = nullptr;


    // -------------------------------------------------------------------------
    // Initialization
    // -------------------------------------------------------------------------

    protected:
        CollabData() = default;
    public:
        virtual ~CollabData() = default; // For polymorphism


    // -------------------------------------------------------------------------
    // Operation Methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Receive an external operation an apply it.
         * Operation is received in its serialized form.
         * This may for instance be used by a network component to apply an
         * operation just received. Operation is in its serialized form since
         * other components doesn't know anything about the concrete operations.
         *
         * Do nothing if unable to unserialize (ex: Type doesn't match content).
         * And return false.
         *
         * \param type Type supposed for this operation.
         * \param buffer Serialized version of the operation.
         * \return True if operation is valid, otherwise, return false.
         */
        virtual bool receiveOperation(const int type,
                                      const std::stringstream& buffer) = 0;

        /**
         * Broadcast an operation to remote replicate.
         * This uses the broadcaster to send operation.
         *
         * Note that this doesn't notify OperationObserver but only send to
         * broadcaster.
         *
         * \param op The operation to broadcast.
         */
        void broadcastOperation(const Operation& op) const {
            this->notifyOperationBroadcaster(op);
        }


    // -------------------------------------------------------------------------
    // OperationObservers Methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Send a local operation to all OperationObservers.
         *
         * Method that modifies data creates an operations that describes this
         * modification. Several components may request to know about these
         * operations, for instance a UI display.
         *
         * \param Reference to the operation.
         */
        void notifyOperationObservers(const Operation& op) const {
            assert(op.getType() != 0); // If 0, you probably forgot to set type
            for(OperationObserver* superman : _operationObservers) {
                superman->notifyOperation(op);
            }
        }

        /**
         * Registers a OperationObserver in this data.
         * Does nothing if this observer is already registered (Returns false).
         * Does nothing if nullptr is given (Returns false).
         *
         * \param observer The observer to add.
         * \return True if added, otherwise, return false.
         */
        bool addOperationObserver(OperationObserver& observer) {
            for(OperationObserver* const obs : _operationObservers) {
                if(obs == &observer) {
                    return false;
                }
            }
            _operationObservers.push_back(&observer);
            return true;
        }

        /**
         * Removes all current operation observers.
         */
        void clearOperationObservers() {
            _operationObservers.clear();
        }

        /**
         * Return the number of operation observer registered in this data.
         *
         * \return Number of registered OperationObserver.
         */
        std::vector<OperationObserver>::size_type sizeOperationObserver() const {
            return _operationObservers.size();
        }


    // -------------------------------------------------------------------------
    // Broadcaster Methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Notify the broadcaster that an operation took place.
         * Do nothing if no broadcaster set.
         *
         * \param Reference to the operation to broadcast.
         */
        void notifyOperationBroadcaster(const Operation& op) const {
            if(_broadcaster != nullptr) {
                _broadcaster->notifyOperation(op);
            }
        }

        /**
         * Associate a broadcaster for this data.
         * If data already has a broadcaster, it is updated with this one.
         */
        void setOperationBroadcaster(OperationObserver& observer) {
            _broadcaster = &observer;
        }

        /**
         * Removes broadcaster for this data.
         */
        void removeOperationBroadcaster() {
            _broadcaster = nullptr;
        }

        /**
         * Check whether a broadcaster is set.
         * Data without a broadcaster means its a local data only.
         *
         * \return True if broadcaster set, otherwise, return false.
         */
        bool hasBroadcaster() const {
            return _broadcaster != nullptr;
        }
};


} // End namespace


