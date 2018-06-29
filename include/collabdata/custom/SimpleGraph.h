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
    // Typedefs / Attributes
    // -------------------------------------------------------------------------

    public:
        typedef std::string UUID;
    private:
        typedef CmRDT::LWWRegister<std::string, Timestamp>        attribute;
        typedef CmRDT::LWWMap<std::string, attribute, Timestamp>  attributesMap;

    private:
        CmRDT::LWWGraph<UUID, attributesMap, Timestamp> _graph;


    // -------------------------------------------------------------------------
    // Operations
    // -------------------------------------------------------------------------

    public:

        /**
         * List all possible Operation on this data.
         * Each enum element is the Operation's unique ID for this data.
         */
        enum class OperationsType : int {
            VERTEX_ADD = 1,
            VERTEX_REMOVE,
            EDGE_ADD,
            EDGE_REMOVE,
            ATTRIBUTE_ADD,
            ATTRIBUTE_REMOVE,
            ATTRIBUTE_SET,
        };

        class VertexAddOperation;
        class VertexRemoveOperation;
        class EdgeAddOperation;
        class EdgeRemoveOperation;
        class AttributeAddOperation;
        class AttributeRemoveOperation;
        class AttributeSetOperation;


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
        void addVertex(const UUID& id);

        /**
         * Remove vertex from the view.
         * Notifies all OperationObservers and broadcaster.
         *
         * \param id The vertex's ID.
         */
        void removeVertex(const UUID& id);

        /**
         * Add edge. (Directional)
         *
         * \param from  Origin vertex id.
         * \param to    Destination vertex id.
         */
        void addEdge(const UUID& from, const UUID& to);

        /**
         * Remove edge.
         *
         * \param from  Origin vertex id.
         * \param to    Destination vertex id.
         */
        void removeEdge(const UUID& from, const UUID& to);

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
        void addAttribute(const UUID& id, const std::string& name,
                          const std::string& value);

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
        void removeAttribute(const UUID& id, const std::string& name);

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
        void setAttribute(const UUID& id, const std::string& name,
                          const std::string& value);


    // -------------------------------------------------------------------------
    // Operation Methods (internal use)
    // -------------------------------------------------------------------------

    private:

        void applyOperation(const VertexAddOperation& op);
        void applyOperation(const VertexRemoveOperation& op);
        void applyOperation(const EdgeAddOperation& op);
        void applyOperation(const EdgeRemoveOperation& op);
        void applyOperation(const AttributeAddOperation& op);
        void applyOperation(const AttributeRemoveOperation& op);
        void applyOperation(const AttributeSetOperation& op);


    // -------------------------------------------------------------------------
    // OperationObserver
    // -------------------------------------------------------------------------

    public:

        /**
         * \copydoc CollabData::receiveOperation
         */
        bool receiveOperation(const int type,
                              const std::stringstream& buffer) override;
};





// /////////////////////////////////////////////////////////////////////////////
// *****************************************************************************
// Nested classes (OPERATIONS)
// *****************************************************************************
// /////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
class SimpleGraph::VertexAddOperation : public Operation {
    private:
        friend SimpleGraph;
        UUID        _vertexID;
        Timestamp   _timestamp = {0};

    public:
        VertexAddOperation() = default;
        VertexAddOperation(const std::string& id, const Timestamp& time)
                : _vertexID(id),
                  _timestamp(time) {
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


// -----------------------------------------------------------------------------
class SimpleGraph::VertexRemoveOperation : public Operation {
    private:
        friend SimpleGraph;
        UUID        _vertexID;
        Timestamp   _timestamp = {0};

    public:

        VertexRemoveOperation() = default;
        VertexRemoveOperation(const std::string& id,
                              const Timestamp& time)
                : _vertexID(id),
                  _timestamp(time) {
            _type = static_cast<int>(OperationsType::VERTEX_REMOVE);
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


// -----------------------------------------------------------------------------
class SimpleGraph::EdgeAddOperation : public Operation {
    private:
        friend SimpleGraph;
        UUID        _fromID;
        UUID        _toID;
        Timestamp   _timestamp = {0};

    public:
        EdgeAddOperation() = default;
        EdgeAddOperation(const UUID& fromID, const UUID& toID,
                         const Timestamp& time)
                : _fromID(fromID),
                  _toID(toID),
                  _timestamp(time) {
            _type = static_cast<int>(OperationsType::EDGE_ADD);
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
        const UUID& fromID() const {
            return _fromID;
        }
        const UUID& toID() const {
            return _toID;
        }
        const Timestamp& timestamp() const {
            return _timestamp;
        }
};


// -----------------------------------------------------------------------------
class SimpleGraph::EdgeRemoveOperation : public Operation {
    private:
        friend SimpleGraph;
        UUID        _fromID;
        UUID        _toID;
        Timestamp   _timestamp = {0};

    public:
        EdgeRemoveOperation() = default;
        EdgeRemoveOperation(const UUID& fromID, const UUID& toID,
                         const Timestamp& time)
                : _fromID(fromID),
                  _toID(toID),
                  _timestamp(time) {
            _type = static_cast<int>(OperationsType::EDGE_REMOVE);
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
        const UUID& fromID() const {
            return _fromID;
        }
        const UUID& toID() const {
            return _toID;
        }
        const Timestamp& timestamp() const {
            return _timestamp;
        }
};


// -----------------------------------------------------------------------------
class SimpleGraph::AttributeAddOperation : public Operation {
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
            _type = static_cast<int>(OperationsType::ATTRIBUTE_ADD);
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


// -----------------------------------------------------------------------------
class SimpleGraph::AttributeRemoveOperation : public Operation {
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
            _type = static_cast<int>(OperationsType::ATTRIBUTE_REMOVE);
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

// -----------------------------------------------------------------------------
class SimpleGraph::AttributeSetOperation : public Operation {
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
                : _vertexID(id),
                  _timestamp(time),
                  _attributeName(name),
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


} // End namespaces


