#pragma once

#include <string>

#include "collabdata/CmRDT/LWWMap.h"
#include "collabdata/CmRDT/LWWGraph.h"
#include "collabdata/CmRDT/LWWRegister.h"

#include "CollabData.h"
#include "Timestamp.h"

namespace collab {


/**
 * \brief
 * A Simple CRDT Directed Graph.
 *
 * Each vertex has a unique string identifier and a map of attributes.
 * (Attributes are string).
 *
 *
 * \author  Constantin Masson
 * \date    May 2018
 */
class SimpleGraph : public CollabData {
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
            VERTEX_ADD = 1,
            VERTEX_DELETE,
            ATTRIBUTE_ADD,
            ATTRIBUTE_REMOVE,
            ATTRIBUTE_SET,
        };

        // ---------------------------------------------------------------------
        class VertexAddOperation : public Operation {
            private:
                friend SimpleGraph;
                UUID        _vertexID;
                Timestamp   _timestamp = {0};

            public:
                VertexAddOperation() = default;
                VertexAddOperation(const std::string& id, const Timestamp& time)
                  : _vertexID(id), _timestamp(time) {
                    _type = static_cast<int>(OperationsType::VERTEX_ADD);
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
                const UUID& vertexID() const {
                    return _vertexID;
                }
                const Timestamp& timestamp() const {
                    return _timestamp;
                }
        };

        // ---------------------------------------------------------------------
        class VertexDeleteOperation : public Operation {
            private:
                friend SimpleGraph;
                UUID        _vertexID;
                Timestamp   _timestamp = {0};

            public:

                VertexDeleteOperation() = default;
                VertexDeleteOperation(const std::string& id,
                                       const Timestamp& time)
                  : _vertexID(id), _timestamp(time) {
                    _type = static_cast<int>(OperationsType::VERTEX_DELETE);
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
                const UUID& vertexID() const {
                    return _vertexID;
                }
                const Timestamp& timestamp() const {
                    return _timestamp;
                }
        };

        // ---------------------------------------------------------------------
        class AttributeAddOperation : public Operation {
            private:
                friend SimpleGraph;
                UUID        _vertexID;
                Timestamp   _timestamp = {0};
                std::string _attributeName;
                std::string _attributeValue;

            public:
                AttributeAddOperation() = default;
                AttributeAddOperation(const std::string& id,
                                      const Timestamp& time,
                                      const std::string& name,
                                      const std::string& value)
                    : _vertexID(id),
                      _timestamp(time),
                      _attributeName(name),
                      _attributeValue(value) {
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
                const UUID& vertexID() const {
                    return _vertexID;
                }
                const Timestamp& timestamp() const {
                    return _timestamp;
                }
                const std::string& attributeName() const {
                    return _attributeName;
                }
                const std::string& attributeValue() const {
                    return _attributeValue;
                }
        };

        // ---------------------------------------------------------------------
        class AttributeRemoveOperation : public Operation {
            private:
                friend SimpleGraph;
                UUID        _vertexID;
                Timestamp   _timestamp = {0};
                std::string _attributeName;

            public:
                AttributeRemoveOperation() = default;
                AttributeRemoveOperation(const std::string& id,
                                         const Timestamp& time,
                                         const std::string& name)
                    : _vertexID(id),
                      _timestamp(time),
                      _attributeName(name) {
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
                const UUID& vertexID() const {
                    return _vertexID;
                }
                const Timestamp& timestamp() const {
                    return _timestamp;
                }
                const std::string& attributeName() const {
                    return _attributeName;
                }
        };

        // ---------------------------------------------------------------------
        class AttributeSetOperation : public Operation {
            private:
                friend SimpleGraph;
                UUID        _vertexID;
                Timestamp   _timestamp = {0};
                std::string _attributeName;
                std::string _newValue;

            public:

                AttributeSetOperation() = default;
                AttributeSetOperation(const std::string& id,
                                      const Timestamp& time,
                                      const std::string& name,
                                      const std::string& nVal)
                  : _vertexID(id), _timestamp(time), _attributeName(name),
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
                const UUID& vertexID() const {
                    return _vertexID;
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

        CmRDT::LWWGraph<UUID, attributesMap, Timestamp> _graph;


    // -------------------------------------------------------------------------
    // Modifiers methods (End-User)
    // -------------------------------------------------------------------------

    public:

        /**
         * Add vertex in the View.
         * Notifies all OperationObservers and broadcaster.
         *
         * \param id The vertex's ID.
         */
        void addVertex(const UUID& id) {
            VertexAddOperation op = {id, Timestamp::now()};
            this->applyOperation(op);
            this->broadcastOperation(op);
        }

        /**
         * Remove vertexs from the view.
         * Notifies all OperationObservers and broadcaster.
         *
         * \param id The vertex's ID.
         */
        void removeVertex(const UUID& id) {
            VertexDeleteOperation op = {id, Timestamp::now()};
            this->applyOperation(op);
            this->broadcastOperation(op);
        }

        /**
         * Add attribute to the given vertex.
         * Notifies all OperationObservers and broadcaster.
         *
         * \note
         * This also re-add the vertex if was deleted.
         * (Or just update 'last-modification' timestamp).
         *
         * \param id    vertex's ID.
         * \param name  Attribute's name.
         * \param value Attribute initial value.
         */
        void addAttribute(const UUID& id,
                          const std::string& name,
                          const std::string& value) {
            AttributeAddOperation op = {id, Timestamp::now(), name, value};
            this->applyOperation(op);
            this->broadcastOperation(op);
        }

        /**
         * Removes attribute from the given vertex.
         * Notifies all OperationObservers and broadcaster.
         *
         * \note
         * This also re-add the vertex if was deleted.
         * (Or just update 'last-modification' timestamp).
         *
         * \param id    vertex's ID.
         * \param name  Attribute's name.
         */
        void removeAttribute(const UUID& id, const std::string& name) {
            AttributeRemoveOperation op = {id, Timestamp::now(), name};
            this->applyOperation(op);
            this->broadcastOperation(op);
        }

        /**
         * Set value of en attribute.
         * Creates this attribute if doesn't exists.
         *
         * \note
         * This also re-add the vertex if was deleted.
         * (Or just update 'last-modification' timestamp).
         *
         * \param id    vertex's ID.
         * \param name  Attribute's name.
         * \param value Attribute's new value.
         */
        void setAttribute(const UUID& id,
                          const std::string& name,
                          const std::string& value) {
            AttributeSetOperation op = {id, Timestamp::now(), name, value};
            this->applyOperation(op);
            this->broadcastOperation(op);
        }


    // -------------------------------------------------------------------------
    // Operation Methods
    // -------------------------------------------------------------------------

    private:

        void applyOperation(const VertexAddOperation& op) {
            auto& tnow      = op.timestamp();
            auto& id        = op.vertexID();
            bool isAdded    = _graph.add_vertex(id, tnow);
            if(isAdded) {
                this->notifyOperationObservers(op);
            }
        }

        void applyOperation(const VertexDeleteOperation& op) {
            auto& tnow      = op.timestamp();
            auto& id        = op.vertexID();
            bool isRemoved  = _graph.remove_vertex(id, tnow);
            if(isRemoved) {
                this->notifyOperationObservers(op);
            }
        }

        void applyOperation(const AttributeAddOperation& op) {
            auto& tnow      = op.timestamp();
            auto& id        = op.vertexID();
            auto& attrName  = op.attributeName();
            auto& attrValue = op.attributeValue();

            // Also add the vertex
            bool isVertexAdded = _graph.add_vertex(id, tnow);
            bool isVertexAlive = _graph.count_vertex(id) > 0 ? true : false;
            if(isVertexAdded) {
                VertexAddOperation vertex_op = {id, tnow};
                this->notifyOperationObservers(vertex_op);
            }

            auto vertex_it  = _graph.crdt_find_vertex(id);
            auto& attrMap   = vertex_it->second.value().content();

            bool isAdded    = attrMap.add(attrName, tnow);
            auto attrElt_it = attrMap.crdt_find(attrName);
            auto& attrElt   = attrElt_it->second.value();

            attrElt.update(attrValue, tnow);
            if(isAdded && isVertexAlive) {
                this->notifyOperationObservers(op);
            }
        }

        void applyOperation(const AttributeRemoveOperation& op) {
            auto& tnow      = op.timestamp();
            auto& id        = op.vertexID();
            auto& attrName  = op.attributeName();

            // Also add the vertex
            bool isVertexAdded = _graph.add_vertex(id, tnow);
            bool isVertexAlive = _graph.count_vertex(id) > 0 ? true : false;
            if(isVertexAdded) {
                VertexAddOperation vertex_op = {id, tnow};
                this->notifyOperationObservers(vertex_op);
            }

            auto vertex_it  = _graph.crdt_find_vertex(id);
            auto& attrMap   = vertex_it->second.value().content();

            bool isRemoved  = attrMap.remove(attrName, tnow);
            if(isRemoved && isVertexAlive) {
                this->notifyOperationObservers(op);
            }
        }

        void applyOperation(const AttributeSetOperation& op) {
            auto& tnow      = op.timestamp();
            auto& id        = op.vertexID();
            auto& newValue  = op.newValue();
            auto& attrName  = op.attributeName();

            // Also add the vertex
            bool isVertexAdded = _graph.add_vertex(id, tnow);
            bool isVertexAlive = (_graph.count_vertex(id) > 0)? true : false;
            if(isVertexAdded) {
                VertexAddOperation vertex_op = {id, tnow};
                this->notifyOperationObservers(vertex_op);
            }

            auto vertex_it  = _graph.crdt_find_vertex(id);
            auto& attrMap   = vertex_it->second.value().content();

            // Also add the attribute
            bool isAttrAdded = attrMap.add(attrName, tnow);
            if(isAttrAdded && isVertexAlive) {
                AttributeAddOperation add_op = {id, tnow, attrName, newValue};
                this->notifyOperationObservers(add_op);
            }

            auto attrElt_it = attrMap.crdt_find(attrName);
            auto& attrElt   = attrElt_it->second.value();

            bool isUpdated  = attrElt.update(newValue, tnow);
            bool attrExists = (attrMap.count(attrName) > 0) ? true : false;
            if(isUpdated && isVertexAlive && attrExists) {
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
                case static_cast<int>(OperationsType::VERTEX_ADD): {
                        VertexAddOperation op;
                        if(!op.unserialize(buffer)) {
                            return false;
                        }
                        applyOperation(op);
                    }
                    break;

                case static_cast<int>(OperationsType::VERTEX_DELETE): {
                        VertexDeleteOperation op;
                        if(!op.unserialize(buffer)) {
                            return false;
                        }
                        applyOperation(op);
                    }
                    break;

                case static_cast<int>(OperationsType::ATTRIBUTE_ADD): {
                        AttributeAddOperation op;
                        if(!op.unserialize(buffer)) {
                            return false;
                        }
                        applyOperation(op);
                    }
                    break;

                case static_cast<int>(OperationsType::ATTRIBUTE_REMOVE): {
                        AttributeRemoveOperation op;
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


