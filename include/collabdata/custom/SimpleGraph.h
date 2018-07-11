#pragma once

#include <string>

#include "collabdata/CmRDT/LWWMap.h"
#include "collabdata/CmRDT/LWWGraph.h"
#include "collabdata/CmRDT/LWWRegister.h"
#include "CollabData.h"
#include "Operation.h"
#include "Timestamp.h"

namespace collab {


/**
 * \brief
 * A Simple Directed Graph using CRDT concurrentcy control.
 *
 * Each vertex has a unique string identifier and a map of attributes.
 * Attributes name and value are both strings.
 *
 * \par Operations
 * The SimpleGraph's operations are available as nested classes.
 *
 * \par Help
 * You may read the documentation from CollabData to understand about
 * Operation, OperationObserver and broadcaster.
 *
 *
 * \see CollabData
 * \see Operation
 * \see OperationObserver
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
        enum Operations : int {
            OPERATION_VERTEX_ADD = 1,
            OPERATION_VERTEX_REMOVE,
            OPERATION_EDGE_ADD,
            OPERATION_EDGE_REMOVE,
            OPERATION_ATTRIBUTE_ADD,
            OPERATION_ATTRIBUTE_REMOVE,
            OPERATION_ATTRIBUTE_SET,
        };

        class VertexAddOperation;
        class VertexRemoveOperation;
        class EdgeAddOperation;
        class EdgeRemoveOperation;
        class AttributeAddOperation;
        class AttributeRemoveOperation;
        class AttributeSetOperation;


    // -------------------------------------------------------------------------
    // Capacity
    // -------------------------------------------------------------------------

    public:

        /**
         * Check whether this graph is empty.
         * Empty means graph has no vertex.
         *
         * \return True if graph is empty, otherwise, returns false.
         */
        bool empty() const noexcept;

        /**
         * Returns number of vertices in the graph.
         *
         * \return Number of vertices in the graph.
         */
        size_t nbVertices() const noexcept;

        /**
         * Returns the total number of edges in the graph.
         *
         * \return Number of edges in the graph.
         */
        size_t nbEdges() const noexcept;


    // -------------------------------------------------------------------------
    // Modifiers methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Add vertex in graph.
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

        /** \copydoc CollabData::receiveOperation */
        bool applyExternOperation(const std::stringstream& buffer) override;
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
        VertexAddOperation(const std::string& id, const Timestamp& time);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
};


// -----------------------------------------------------------------------------
class SimpleGraph::VertexRemoveOperation : public Operation {
    private:
        friend SimpleGraph;
        UUID        _vertexID;
        Timestamp   _timestamp = {0};

    public:
        VertexRemoveOperation() = default;
        VertexRemoveOperation(const std::string& id, const Timestamp& time);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
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
                         const Timestamp& time);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        const UUID& fromID() const;
        const UUID& toID() const;
        const Timestamp& timestamp() const;
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
                            const Timestamp& time);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        const UUID& fromID() const;
        const UUID& toID() const;
        const Timestamp& timestamp() const;
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
        AttributeAddOperation(const std::string& id, const Timestamp& time,
                              const std::string& name, const std::string& value);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
        const std::string& attributeName() const;
        const std::string& attributeValue() const;
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
        AttributeRemoveOperation(const std::string& id, const Timestamp& time,
                                 const std::string& name);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
        const std::string& attributeName() const;
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
        AttributeSetOperation(const std::string& id, const Timestamp& time,
                              const std::string& name, const std::string& nVal);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
        const std::string& attributeName() const;
        const std::string& newValue() const;
};


} // End namespaces


