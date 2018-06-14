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
 * This is a custom CollabData made specially for the AToMPM software.
 * ViewMDE is a MDE model (Internally represented as a graph. Each vertex has
 * a map of attributes.). ALl methods are CRDT.
 *
 * \todo
 * Documentation to write.
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
            private:
                friend ViewMDE;
                UUID        _elementID;
                Timestamp   _timestamp = {0};

            public:
                ElementAddOperation() = default;
                ElementAddOperation(const std::string& id, const Timestamp& time)
                  : _elementID(id), _timestamp(time) {
                    _type = static_cast<int>(OperationsType::ELEMENT_ADD);
                }

            public:
                bool serialize(std::stringstream& buffer) const override {
                    // TODO
                    return false;
                }
                bool unserialize(const std::stringstream& buffer) override {
                    // TODO
                    return false;
                }

            public:
                const UUID& elementID() const {
                    return _elementID;
                }
                const Timestamp& timestamp() const {
                    return _timestamp;
                }
        };

        // ---------------------------------------------------------------------
        class ElementDeleteOperation : public Operation {
            private:
                friend ViewMDE;
                UUID        _elementID;
                Timestamp   _timestamp = {0};

            public:

                ElementDeleteOperation() = default;
                ElementDeleteOperation(const std::string& id,
                                       const Timestamp& time)
                  : _elementID(id), _timestamp(time) {
                    _type = static_cast<int>(OperationsType::ELEMENT_DELETE);
                }

            public:
                bool serialize(std::stringstream& buffer) const override {
                    // TODO
                    return false;
                }
                bool unserialize(const std::stringstream& buffer) override {
                    // TODO
                    return false;
                }

            public:
                const UUID& elementID() const {
                    return _elementID;
                }
                const Timestamp& timestamp() const {
                    return _timestamp;
                }
        };

        // ---------------------------------------------------------------------
        class AttributeSetOperation : public Operation {
            private:
                friend ViewMDE;
                UUID        _elementID;
                Timestamp   _timestamp = {0};
                std::string _attributeName;
                std::string _newValue;

            public:

                AttributeSetOperation() = default;
                AttributeSetOperation(const std::string& id,
                                      const Timestamp& time,
                                      const std::string& name,
                                      const std::string& nVal)
                  : _elementID(id), _timestamp(time), _attributeName(name),
                    _newValue(nVal) {
                    _type = static_cast<int>(OperationsType::ATTRIBUTE_SET);
                }

            public:
                bool serialize(std::stringstream& buffer) const override {
                    // TODO
                    return false;
                }
                bool unserialize(const std::stringstream& buffer) override {
                    // TODO
                    return false;
                }

            public:
                const UUID& elementID() const {
                    return _elementID;
                }
                const Timestamp& timestamp() const {
                    return _timestamp;
                }
                const std::string& attributeName() const {
                    return _attributeName;
                }
                const std::string& newValue() const {
                    return _newValue;
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
            ElementAddOperation op = {id, Timestamp::now()};
            this->applyOperation(op);
            this->broadcastOperation(op);
        }

        /**
         * Remove elements from the view.
         * Notifies all OperationObservers.
         *
         * \param id The element's ID.
         */
        void removeElement(const UUID& id) {
            ElementDeleteOperation op = {id, Timestamp::now()};
            this->applyOperation(op);
            this->broadcastOperation(op);
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
            auto tnow = Timestamp::now();
            AttributeSetOperation op = {eltID, tnow, name, value};
            this->applyOperation(op);
            this->broadcastOperation(op);
        }


    // -------------------------------------------------------------------------
    // Operation Methods
    // -------------------------------------------------------------------------

    private:

        void applyOperation(const ElementAddOperation& op) {
            auto& tnow      = op.timestamp();
            auto& id        = op.elementID();
            bool isAdded    = _modelMDE.add_vertex(id, tnow);
            if(isAdded) {
                this->notifyOperationObservers(op);
            }
        }

        void applyOperation(const ElementDeleteOperation& op) {
            auto& tnow      = op.timestamp();
            auto& id        = op.elementID();
            bool isRemoved  = _modelMDE.remove_vertex(id, tnow);
            if(isRemoved) {
                this->notifyOperationObservers(op);
            }
        }

        void applyOperation(const AttributeSetOperation& op) {
            auto& tnow      = op.timestamp();
            auto& id        = op.elementID();
            auto& newValue  = op.newValue();
            auto& attrName  = op.attributeName();

            _modelMDE.add_vertex(id, tnow);
            auto vertex_it = _modelMDE.find_vertex(id);
            auto& attrMap = vertex_it->second.content();

            attrMap.add(attrName, tnow);
            auto attrElt_it = attrMap.find(attrName);
            auto& attrElt = attrElt_it->second;

            bool isUpdated = attrElt.update(newValue, tnow);
            if(isUpdated) {
                this->notifyOperationObservers(op);
            }
        }


    // -------------------------------------------------------------------------
    // OperationObserver
    // -------------------------------------------------------------------------

    public:

        bool receiveOperation(const int type,
                              const std::stringstream& buffer) override {
            switch(type) {
                case static_cast<int>(OperationsType::ELEMENT_ADD): {
                        ElementAddOperation op;
                        if(!op.unserialize(buffer)) {
                            return false;
                        }
                        applyOperation(op);
                    }
                    break;

                case static_cast<int>(OperationsType::ELEMENT_DELETE): {
                        ElementDeleteOperation op;
                        if(!op.unserialize(buffer)) {
                            return false;
                        }
                        applyOperation(op);
                    }
                    break;

                case static_cast<int>(OperationsType::ATTRIBUTE_SET): {
                        AttributeSetOperation op;
                        if(!op.unserialize(buffer)) {
                            return false;
                        }
                        applyOperation(op);
                    }
                    break;

                default:
                    return false;
            }

            return true;
        }
};


} // End namespaces


