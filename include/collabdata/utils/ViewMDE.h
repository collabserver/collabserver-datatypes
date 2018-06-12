#pragma once

#include <string>

#include "CollabData.h"
#include "Timestamp.h"

#include "collabdata/CmRDT/LWWMap.h"
#include "collabdata/CmRDT/LWWGraph.h"
#include "collabdata/CmRDT/LWWRegister.h"

namespace collab {


/**
 * \brief
 * View data structure for Model Driven Engineering.
 *
 * ViewMDE is built on top of collab CRDTs.
 *
 * \todo
 * TODO Documentation
 *
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
class ViewMDE : public CollabData {
    // -------------------------------------------------------------------------
    // Typedefs
    // -------------------------------------------------------------------------

    public:
        typedef std::string UUID;

    private:
        typedef CmRDT::LWWRegister<std::string, Timestamp>        attribute;
        typedef CmRDT::LWWMap<std::string, attribute, Timestamp>  attributesMap;


    // -------------------------------------------------------------------------
    // Operations
    // -------------------------------------------------------------------------

    public:

        /**
         * List all possible Operation on this data.
         */
        enum class OperationsType : int {
            ELEMENT_ADD = 1,
            ELEMENT_DELETE,
            ATTRIBUTE_SET,
        };

        // ---------------------------------------------------------------------
        class ElementAddOperation : public Operation {
            public:
                friend ViewMDE;
                const UUID          elementID;
                const Timestamp     timestamp;

            public:

                ElementAddOperation(const std::string& id, const Timestamp& time)
                  : elementID(id), timestamp(time) {
                    _type = static_cast<int>(OperationsType::ELEMENT_ADD);
                }

            public:
                bool serialize(std::stringstream& buffer) const {
                    // TODO
                    return false;
                }
                bool unserialize(std::stringstream& buffer) {
                    // TODO
                    return false;
                }
        };

        // ---------------------------------------------------------------------
        class ElementDeleteOperation : public Operation {
            public:
                friend ViewMDE;
                const UUID          elementID;
                const Timestamp     timestamp;

            public:

                ElementDeleteOperation(const std::string& id,
                                       const Timestamp& time)
                  : elementID(id), timestamp(time) {
                    _type = static_cast<int>(OperationsType::ELEMENT_DELETE);
                }

            public:
                bool serialize(std::stringstream& buffer) const {
                    // TODO
                    return false;
                }
                bool unserialize(std::stringstream& buffer) {
                    // TODO
                    return false;
                }
        };

        // ---------------------------------------------------------------------
        class AttributeSetOperation : public Operation {
            public:
                friend ViewMDE;
                const UUID          elementID;
                const Timestamp     timestamp;
                const std::string   attributeName;
                const std::string   newValue;
                const std::string   oldValue;

            public:

                AttributeSetOperation(const std::string& id,
                                      const Timestamp& time,
                                      const std::string& name,
                                      const std::string& nVal,
                                      const std::string& oVal)
                  : elementID(id), timestamp(time), attributeName(name),
                    newValue(nVal), oldValue(oVal) {
                    _type = static_cast<int>(OperationsType::ATTRIBUTE_SET);
                }

            public:
                bool serialize(std::stringstream& buffer) const {
                    // TODO
                    return false;
                }
                bool unserialize(std::stringstream& buffer) {
                    // TODO
                    return false;
                }

        };


    // -------------------------------------------------------------------------
    // Attributes
    // -------------------------------------------------------------------------

    private:

        CmRDT::LWWGraph<UUID, attributesMap, Timestamp> _modelMDE;


    // -------------------------------------------------------------------------
    // Modifiers methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Add element in the View.
         * Notifies all OperationObservers.
         *
         * \param id The element's ID.
         */
        void addElement(const UUID& id) {
            const ElementAddOperation op = _addElement(id);
            this->notifyOperationBroadcaster(op);
        }

        /**
         * Remove elements from the view.
         * Notifies all OperationObservers.
         *
         * \param id The element's ID.
         */
        void removeElement(const UUID& id) {
            const ElementDeleteOperation op = _removeElement(id);
            this->notifyOperationBroadcaster(op);
        }

        /**
         * Set value of en attribute.
         * Creates this attribute if doesn't exists.
         * If the eltID doesn't exists or has been removed, this also add/re-add
         * the vertex.
         *
         * \param eltID Element's ID.
         * \param name  Attribute's name.
         * \param value Attribute's new value.
         */
        void setAttribute(const UUID& eltID,
                          const std::string& name,
                          const std::string& value) {
            AttributeSetOperation op = _setAttribute(eltID, name, value);
            this->notifyOperationBroadcaster(op);
        }


    // -------------------------------------------------------------------------
    // Internal modifiers methods
    // -------------------------------------------------------------------------

    private:

        // Add element. Notify observers if elt added.
        ElementAddOperation _addElement(const UUID& id) {
            auto tnow = Timestamp::now();
            bool isAdded = _modelMDE.add_vertex(id, tnow);
            ElementAddOperation op = {id, tnow};
            if(isAdded) {
                this->notifyOperationObservers(op);
            }
            return op;
        }

        ElementDeleteOperation _removeElement(const UUID& id) {
            auto tnow = Timestamp::now();
            bool isRemoved = _modelMDE.remove_vertex(id, tnow);
            ElementDeleteOperation op = {id, tnow};
            if(isRemoved) {
                this->notifyOperationObservers(op);
            }
            return op;
        }

        AttributeSetOperation _setAttribute(const UUID& eltID,
                                            const std::string& name,
                                            const std::string& value) {
            auto tnow = Timestamp::now();

            _modelMDE.add_vertex(eltID, tnow);
            auto vertex_it = _modelMDE.find_vertex(eltID);
            auto& attrMap = vertex_it->second.content();

            attrMap.add(name, tnow);
            auto attrElt_it = attrMap.find(name);
            auto& attrElt = attrElt_it->second;

            std::string oldValue = attrElt.query();
            bool isUpdated = attrElt.update(value, tnow);
            AttributeSetOperation op = {eltID, tnow, name, oldValue, value};
            if(isUpdated) {
                this->notifyOperationObservers(op);
            }

            return op;
        }


    // -------------------------------------------------------------------------
    // OperationObserver
    // -------------------------------------------------------------------------

    public:

        void applyOperation(const Operation& op) {
            // TODO
        }

        void applyOperation(const int type, std::stringstream& buffer) {
            // TODO Get operation by type
            // Unserialize
            // Call applyOperation
        }
};


} // End namespaces


