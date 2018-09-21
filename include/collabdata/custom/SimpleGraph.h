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
 * \par Concurrency
 * SimpleGraph is a CollabData.
 * All operations follow the CRDT features.
 *
 * \par Operations
 * The SimpleGraph's operations are available as nested classes.
 *
 * \par Help
 * You may read the documentation from CollabData to understand about
 * Operation, OperationObserver and broadcaster.
 *
 * \par Thread-Safe
 * All operations are thread safe.
 * Note that thread-safe and collaboration are not the same.
 * Collaboration allows several users to apply operations that are logically
 * concurrent. Thread safe allows operations to be applied from several threads.
 *
 *
 * \see CollabData
 * \see Operation
 * \see OperationObserver
 * \see OperationHandler
 * \see Timestamp
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
        unsigned int _localID = 0; // Used for Timestamp userID


    // -------------------------------------------------------------------------
    // Constructors / Init / Builder
    // -------------------------------------------------------------------------

    public:
        /**
         * Create (On Stack) a SimpleGraph for the given user.
         *
         * \param localID ID of the local user working on this graph.
         */
        static SimpleGraph build(unsigned int localID);

        /**
         * Create (On heap) a SimpleGraph for the given user.
         *
         * \warning
         * This call new, you must manually delete the memory.
         *
         * \param localID ID of the local user working on this graph.
         */
        static SimpleGraph* buildNew(unsigned int localID);

    private:

        /**
         * Create a SimpleGraph. A unique userID is given to represents
         * the local user ID. It is used by Timestamp.
         * (ID may be set and returned by a collab-server for instance).
         *
         * \bug WARNING
         * Because of initialization of base classes, you must call
         * Timestamp::setEffectiveID(localID) before creating your SimpleGraph.
         * This is due to the default timestamp created by LWWSet and LWWMap
         * for the 'reset' feature.
         * WARNING: Timestamp implementation should NOT be displayed to end
         * user. This is why SimpleGraph constructor is PRIVATE and
         * SimpleGraph should be created by its builder.
         *
         * \param localID ID of the user.
         */
        SimpleGraph(unsigned int localID) : _localID(localID) {}


    // -------------------------------------------------------------------------
    // Operations
    // -------------------------------------------------------------------------

    public:

        class OpHandler;

        /**
         * List all possible Operation on this data.
         * Each enum element is the Operation's unique ID for this data.
         */
        enum Operations : unsigned int {
            OPERATION_VERTEX_ADD = 1, // Important, first is NOT 0
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
    // Lookup methods
    // -------------------------------------------------------------------------

    public:

        /**
         * Get an iterator over the whole set of vertices.
         *
         * \return Iterator of vertex.
         */
        VertexIterator vertices() const;

        /**
         * Get the vertex descriptor for the given vertex ID.
         * Throw exception std::out_of_range if no vertex for this id.
         *
         * \param id Vertex to look for.
         * \return VertexDescriptor.
         */
        VertexDescriptor at(const UUID& id) const;

        /**
         * Check whether there is a vertex with this id.
         *
         * \param id Vertex id.
         * \return True if has vertex with this id, otherwise, return false.
         */
        bool hasVertex(const UUID& id) const;

        /**
         * Check whether an edge exists.
         *
         * \param from  Origin vertex.
         * \param to    Destination vertex.
         * \return      True if exists, otherwise, return false.
         */
        bool hasEdge(const UUID& from, const UUID& to) const;


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
        bool applyExternOperation(unsigned int id,
                                  const std::string& buffer) override;
};


// /////////////////////////////////////////////////////////////////////////////
// Nested classes (Iterators)
// /////////////////////////////////////////////////////////////////////////////


/**
 * Iterator for graph's vertices.
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
        VertexIterator(const _Graph& graph) : _data(graph),
                                              _it(graph.cbegin()) {}

    public:

        /**
         * Return the current element in iterator.
         * This has undefined behavior if moveNext already returned false.
         *
         * \return Current element.
         */
        VertexDescriptor current() const;

        /**
         * Move iterator to the next element.
         * Returns false if current was the last element.
         *
         * \return True if successfully moved to next element.
         */
        bool moveNext();
};


/**
 * Iterator for vertex edges of a vertex.
 * Read-only access.
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

        /** \copydoc SimpleGraph::VertexIterator::current */
        const UUID& current() const;

        /** \copydoc SimpleGraph::VertexIterator::moveNext */
        bool moveNext();
};


/**
 * Iterator for vertex content attributes.
 * Read-only access.
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

        /** \copydoc SimpleGraph::VertexIterator::current */
        const AttributeDescriptor current() const;

        /** \copydoc SimpleGraph::VertexIterator::moveNext */
        bool moveNext();
};


// /////////////////////////////////////////////////////////////////////////////
// Nested classes (Descriptors)
// /////////////////////////////////////////////////////////////////////////////


/**
 * Describe content of a vertex.
 * Read-only access.
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
        const std::string& name() const { return _name; }
        const std::string& value() const { return _value; }
};


/**
 * Describe content of a vertex.
 * Read-only access.
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
                         const _EdgeSet& edges);

    public:

        /**
         * Get id of the vertex.
         *
         * \return Vertex unique ID.
         */
        const UUID& id() const;

        /**
         * Get the edges iterator for this vertex.
         */
        EdgeIterator edges() const;

        /**
         * Get the attributes iterator for this vertex.
         */
        AttributeIterator attributes() const;
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

        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        unsigned int getType() const override { return OPERATION_VERTEX_ADD; }

        const UUID& getVertexID() const { return _vertexID; }
        const Timestamp& getTimestamp() const { return _timestamp; }
};


// -----------------------------------------------------------------------------
class SimpleGraph::VertexRemoveOperation : public Operation {
    private:
        UUID        _vertexID;
        Timestamp   _timestamp = {0};

    public:
        VertexRemoveOperation() = default;
        VertexRemoveOperation(const std::string& id, const Timestamp& time);

        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        unsigned int getType() const override { return OPERATION_VERTEX_REMOVE; }

        const UUID& getVertexID() const { return _vertexID; }
        const Timestamp& getTimestamp() const { return _timestamp; }
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

        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        unsigned int getType() const override { return OPERATION_EDGE_ADD; }

        const UUID& getFromID() const { return _fromID; }
        const UUID& getToID() const { return _toID; }
        const Timestamp& getTimestamp() const { return _timestamp; }
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

        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        unsigned int getType() const override { return OPERATION_EDGE_REMOVE; }

        const UUID& getFromID() const { return _fromID; }
        const UUID& getToID() const { return _toID; }
        const Timestamp& getTimestamp() const { return _timestamp; }
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

        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        unsigned int getType() const override { return OPERATION_ATTRIBUTE_ADD; }

        const UUID& getVertexID() const { return _vertexID; }
        const Timestamp& getTimestamp() const { return _timestamp; }
        const std::string& getAttributeName() const { return _attributeName; }
        const std::string& getAttributeValue() const { return _attributeValue; }
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

        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        unsigned int getType() const override { return OPERATION_ATTRIBUTE_REMOVE; }

        const UUID& getVertexID() const { return _vertexID; }
        const Timestamp& getTimestamp() const { return _timestamp; }
        const std::string& getAttributeName() const { return _attributeName; }
};


// -----------------------------------------------------------------------------
class SimpleGraph::AttributeSetOperation : public Operation {
    private:
        UUID        _vertexID;
        Timestamp   _timestamp = {0};
        std::string _attributeName;
        std::string _attributeValue;

    public:

        AttributeSetOperation() = default;
        AttributeSetOperation(const std::string& id, const Timestamp& time,
                              const std::string& name, const std::string& nVal);

        bool serialize(std::stringstream& buffer) const override;
        bool unserialize(const std::stringstream& buffer) override;
        void accept(OperationHandler& handler) const override;
        unsigned int getType() const override { return OPERATION_ATTRIBUTE_SET; }

        const UUID& getVertexID() const { return _vertexID; }
        const Timestamp& getTimestamp() const { return _timestamp; }
        const std::string& getAttributeName() const { return _attributeName; }
        const std::string& getAttributeValue() const { return _attributeValue; }
};


} // End namespaces


