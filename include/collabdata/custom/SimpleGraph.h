#pragma once

#include <string>

#include "collabdata/CmRDT/LWWMap.h"
#include "collabdata/CmRDT/LWWGraph.h"
#include "collabdata/CmRDT/LWWSet.h"
#include "collabdata/CmRDT/LWWRegister.h"
#include "Timestamp.h"
#include "CollabData.h"
#include "Operation.h"
#include "OperationHandler.h"

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
        typedef CmRDT::LWWSet<UUID, Timestamp>                    _EdgeSet;
        typedef CmRDT::LWWRegister<std::string, Timestamp>        _Attribute;
        typedef CmRDT::LWWMap<std::string, _Attribute, Timestamp> _AttributeMap;
        typedef CmRDT::LWWGraph<UUID, _AttributeMap, Timestamp>   _Graph;

    public:
        class VertexIterator;
        class EdgeIterator;
        class AttributeIterator;
        class VertexDescriptor;
        class AttributeDescriptor;

    private:
        _Graph _graph;


    // -------------------------------------------------------------------------
    // Operations
    // -------------------------------------------------------------------------

    public:

        class OpHandler;

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
    // Query methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Get an iterator over the whole set of vertices.
         *
         * \return Iterator of vertex.
         */
        VertexIterator vertices();


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
// Nested classes (Descriptors - Iterators)
// /////////////////////////////////////////////////////////////////////////////

/**
 * Iterator over edges of a vertex.
 * Read only access.
 */
class SimpleGraph::EdgeIterator {
    private:
        const _EdgeSet&             _data;
        _EdgeSet::const_iterator    _it;
        bool                        _reset = true;

    private:
        friend SimpleGraph;
        EdgeIterator(const _EdgeSet& set) : _data(set), _it(set.cbegin()) {}

    public:

        /**
         * Return the current edge id.
         * This has undefined behavior if moveNext already returned false.
         *
         * \return Current element.
         */
        const UUID& current() const {
            return *_it;
        }

        /**
         * Move iterator to the next element.
         * Returns false if current was the last element.
         *
         * \return True if successfully moved to next element.
         */
        bool moveNext() {
            if(_reset) {
                _reset = false;
                return _it != _data.cend();
            }
            ++_it;
            return _it != _data.cend();
        }
};


/**
 * TODO
 */
class SimpleGraph::AttributeDescriptor {
    private:
        const std::string& _name;
        const std::string& _value;
    private:
        friend SimpleGraph;
        AttributeDescriptor(const std::string& name, const std::string& value)
                : _name(name), _value(value) {}
    public:
        const std::string& name() const {
            return _name;
        }
        const std::string& value() const {
            return _value;
        }
};


/**
 * TODO Documentation
 */
class SimpleGraph::AttributeIterator {
    private:
        const _AttributeMap&            _data;
        _AttributeMap::const_iterator   _it;
        bool                            _reset = true;

    private:
        friend SimpleGraph;
        AttributeIterator(const _AttributeMap& map)
            : _data(map), _it(map.begin()) {}

    public:

        /**
         * Return the current attribute data.
         * This has undefined behavior if moveNext already returned false.
         *
         * \return Current attribute.
         */
        const AttributeDescriptor current() const {
            auto& current = *_it;
            return {current.first, current.second.query()};
        }

        /**
         * Move iterator to the next element.
         * Returns false if current was the last element.
         *
         * \return True if successfully moved to next element.
         */
        bool moveNext() {
            if(_reset) {
                _reset = false;
                return _it != _data.cend();
            }
            ++_it;
            return _it != _data.cend();
        }

};


/**
 * Describe content of a vertex.
 */
class SimpleGraph::VertexDescriptor {
    private:
        const UUID&             _id;
        const _AttributeMap&    _content;
        const _EdgeSet&         _edges;

    private:
        friend SimpleGraph;
        VertexDescriptor(const UUID& id,
                         const _AttributeMap& attributes,
                         const _EdgeSet& edges)
                : _id(id), _content(attributes), _edges(edges) {
        }

    public:

        /**
         * Get id of the vertex.
         *
         * \return Vertex unique ID.
         */
        const UUID& id() const {
            return _id;
        }

        /**
         * Get the edges iterator for this vertex.
         */
        EdgeIterator edges() {
            return EdgeIterator(_edges);
        }

        /**
         * Get the attributes iterator for this vertex.
         */
        AttributeIterator attributes() {
            return AttributeIterator(_content);
        }
};


/**
 * Iterator over graph vertices.
 *
 * Because of the internal CRDT properties of SimpleGraph, iterator allows
 * only read-only access.
 * To update the content of SimpleGraph, use the appropriate methods.
 */
class SimpleGraph::VertexIterator {
    private:
        const _Graph&           _data;
        _Graph::const_iterator  _it;
        bool                    _reset = true;

    private:
        friend SimpleGraph;
        VertexIterator(_Graph& graph) : _data(graph), _it(graph.cbegin()) {}

    public:

        /**
         * Return the current element in iterator.
         * This has undefined behavior if moveNext already returned false.
         *
         * \return Current VertexDescriptor.
         */
        VertexDescriptor current() const {
            auto& current = *_it;
            return {current.first,
                    current.second.content(),
                    current.second.edges()};
        }

        /**
         * Move iterator to the next element.
         * Returns false if current was the last element.
         *
         * \return True if successfully moved to next element.
         */
        bool moveNext() {
            if(_reset) {
                _reset = false;
                return _it != _data.cend();
            }
            ++_it;
            return _it != _data.cend();
        }
};


// /////////////////////////////////////////////////////////////////////////////
// Nested classes (Operations Handler)
// /////////////////////////////////////////////////////////////////////////////

class SimpleGraph::OpHandler : public OperationHandler {
    protected:
        OpHandler() = default;
        OpHandler(const OpHandler& other) = default;
        OpHandler& operator=(const OpHandler& other) = default;
    public:
        virtual void handleOperation(const VertexAddOperation& op) = 0;
        virtual void handleOperation(const VertexRemoveOperation& op) = 0;
        virtual void handleOperation(const EdgeAddOperation& op) = 0;
        virtual void handleOperation(const EdgeRemoveOperation& op) = 0;
        virtual void handleOperation(const AttributeAddOperation& op) = 0;
        virtual void handleOperation(const AttributeRemoveOperation& op) = 0;
        virtual void handleOperation(const AttributeSetOperation& op) = 0;
};


// /////////////////////////////////////////////////////////////////////////////
// Nested classes (Operations)
// /////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
class SimpleGraph::VertexAddOperation : public Operation {
    private:
        UUID        _vertexID;
        Timestamp   _timestamp = {0};

    public:
        VertexAddOperation() = default;
        VertexAddOperation(const std::string& id, const Timestamp& time);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
};


// -----------------------------------------------------------------------------
class SimpleGraph::VertexRemoveOperation : public Operation {
    private:
        UUID        _vertexID;
        Timestamp   _timestamp = {0};

    public:
        VertexRemoveOperation() = default;
        VertexRemoveOperation(const std::string& id, const Timestamp& time);
        int getType() const override;
        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
};


// -----------------------------------------------------------------------------
class SimpleGraph::EdgeAddOperation : public Operation {
    private:
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
        void accept(OperationHandler& handler) const override;
        const UUID& fromID() const;
        const UUID& toID() const;
        const Timestamp& timestamp() const;
};


// -----------------------------------------------------------------------------
class SimpleGraph::EdgeRemoveOperation : public Operation {
    private:
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
        void accept(OperationHandler& handler) const override;
        const UUID& fromID() const;
        const UUID& toID() const;
        const Timestamp& timestamp() const;
};


// -----------------------------------------------------------------------------
class SimpleGraph::AttributeAddOperation : public Operation {
    private:
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
        void accept(OperationHandler& handler) const override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
        const std::string& attributeName() const;
        const std::string& attributeValue() const;
};


// -----------------------------------------------------------------------------
class SimpleGraph::AttributeRemoveOperation : public Operation {
    private:
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
        void accept(OperationHandler& handler) const override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
        const std::string& attributeName() const;
};


// -----------------------------------------------------------------------------
class SimpleGraph::AttributeSetOperation : public Operation {
    private:
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
        void accept(OperationHandler& handler) const override;
        const UUID& vertexID() const;
        const Timestamp& timestamp() const;
        const std::string& attributeName() const;
        const std::string& newValue() const;
};


} // End namespaces


