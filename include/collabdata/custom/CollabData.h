#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "Operation.h"
#include "OperationObserver.h"

namespace collab {

/**
 * \brief
 * Abstract class for data built on top of CRDT primitives.
 *
 * CollabData is meant to be used in distributed environments.
 * It is a 'end-user' data that internally uses primitive CRDTs to deal
 * with concurrent changes without any synchronization or lock.
 *
 * Don't confuse with thread safe. CollabData is not thread safe.
 * Several users may work at the same time on the same CollabData without any
 * conflicts, lock and synchronization. However, the local replicate applies
 * modifications in one thread (For instance, a server that receives
 * modifications from several users but applies them on one thread).
 * CollabData deals with the fact that operations themselves may be concurrent.
 * (ex: Bob deletes a component that Alice has modified at the same time).
 *
 *
 * \par Methods rules
 * All methods on this data are associative, commutative, and idempotent.
 *  - Associative
 *      (a + b) + c = a + (b + c)
 *  - Commutative
 *      (a + b) = (b + a)
 *  - idempotent
 *      (a + b) = (a + b) + b
 *
 * \par Operation
 * Operation describes an atomic change on the data. Your CollabData has its
 * own set of Operation that you defined and are specific for this data.
 * Each operation is a class that implements Operation interface and describes
 * a specific modification. (ex: add element in map). You may defines
 * operation as nested classes of your data.
 *
 * \par OperationObserver
 * Any modification on the data generates an Operation that describes this
 * atomic change and notifies all its observers. These OperationObservers
 * then receive the operation and may process is as they wish. For instance,
 * update a GUI or update a log of operations.
 * Note that OperationObserver shouldn't be used in order to send the operation
 * to other replicates, use the broadcaster instead.
 * \par
 * IMPORTANT NOTE: OperationObserver are notified only of operations that
 * are valid from the end-user point of view.
 * Methods may have semantic dependencies, for instance, to modify an element,
 * it must be created first. (Causality preservation). To remove
 * any synchronization mechanism, all methods are technically valid. Internally,
 * CRDTs are totally commutative by design. (for instance, element may be added
 * temporary in order to apply the modification)
 * OperationObserver creates the borderland between internal CRDT (Where
 * everything is valid) and the end-user semantic which requires causality
 * preservation.
 *
 * \par Broadcaster
 * This is a special kind OperationObserver. They technically both implement
 * OperationObserver and receive notification of operations.
 * However, Broadcaster is notified only for Operations generated by the local
 * user in order to send them to other replicates.
 * More over, broadcaster sent all operation regardless causality preservation.
 * This is required since all replicates must apply the exact same set of
 * operations locally.
 * Using normal OperationObserver for this task will result in a possible loop
 * since external modification will be sent back to its creator.
 * How broadcaster actually deals with operation broadcasting is unknown here.
 * This is an implementation detail that CollabData is not aware of.
 * You may use BSD sockets, any network communication or magic birds, CollabData
 * only knows about a broadcaster that listen to its changes.
 *
 * \par How to implement your custom CollabData
 *  - Create your data that implements CollabData
 *  - Build your internal data on top of CRDTs primitives (ex: CmRDT::LWWMap)
 *  - Create a set of operations for your data
 *
 * \par How to use you custom CollabData
 *  - Create class that implements OperationObserver
 *  - Deal with each possible Operation (Update UI, Insert in Database...)
 *  - Register your OperationObservers in your CollabData
 *  - Create a broadcaster that implements OperationObserver
 *  - Register broadcaster in your CollabData
 *
 *
 * \see Operation
 * \see OperationObserver
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
    CollabData(const CollabData& other) = default;
    CollabData& operator=(const CollabData& other) = default;

   public:
    virtual ~CollabData() = default;  // For polymorphism

    // -------------------------------------------------------------------------
    // Operation Methods
    // -------------------------------------------------------------------------

   public:
    /**
     * Apply an operation received from external component.
     * Operation is received in its serialized form.
     * Do nothing if unable to unserialize and return false.
     * This doesn't notify the broadcaster but only the Operation Observers.
     *
     * This may for instance be used by a network component to apply an
     * operation just received. Operation is in its serialized form since
     * other components doesn't know anything about the concrete operations.
     * (Only concrete CollabData implementation knowns)
     *
     * \param id Operation's ID.
     * \param buffer Serialized version of the operation.
     * \return True if operation is valid, otherwise, return false.
     */
    virtual bool applyExternOperation(unsigned int id, const std::string& buffer) = 0;

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
        assert(op.getType() != 0);  // If 0, you probably forgot to set type

        for (OperationObserver* superman : _operationObservers) {
            assert(superman != nullptr);
            superman->onOperation(op);
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
        for (OperationObserver* const obs : _operationObservers) {
            if (obs == &observer) {
                return false;
            }
        }
        _operationObservers.push_back(&observer);
        return true;
    }

    /**
     * Removes all current operation observers.
     */
    void clearOperationObservers() { _operationObservers.clear(); }

    /**
     * Returns the number of operation observer registered in this data.
     *
     * \return Number of registered OperationObserver.
     */
    std::vector<OperationObserver>::size_type sizeOperationObserver() const { return _operationObservers.size(); }

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
        if (_broadcaster != nullptr) {
            _broadcaster->onOperation(op);
        }
    }

    /**
     * Associate a broadcaster for this data.
     * If data already has a broadcaster, it is updated with this one.
     */
    void setOperationBroadcaster(OperationObserver& observer) { _broadcaster = &observer; }

    /**
     * Removes broadcaster for this data.
     */
    void removeOperationBroadcaster() { _broadcaster = nullptr; }

    /**
     * Check whether a broadcaster is set.
     * Data without a broadcaster means its a local data only.
     *
     * \return True if broadcaster set, otherwise, return false.
     */
    bool hasBroadcaster() const { return _broadcaster != nullptr; }
};

}  // namespace collab
