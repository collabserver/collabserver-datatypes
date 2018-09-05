#include "collabdata/custom/SimpleGraph.h"

#include <sstream>

namespace collab {


// -----------------------------------------------------------------------------
// Constructor / Init
// -----------------------------------------------------------------------------

SimpleGraph::SimpleGraph(unsigned int localID) {
    Timestamp::setEffectiveID(localID);
    _localID = localID;
}


// -----------------------------------------------------------------------------
// Capacity
// -----------------------------------------------------------------------------

size_t SimpleGraph::nbVertices() const noexcept {
    return _graph.size_vertex();
}

size_t SimpleGraph::nbEdges() const noexcept {
    return _graph.size_edges();
}


// -----------------------------------------------------------------------------
// Lookup methods
// -----------------------------------------------------------------------------

SimpleGraph::VertexIterator SimpleGraph::vertices() const {
    return VertexIterator(_graph);
}

SimpleGraph::VertexDescriptor SimpleGraph::at(const SimpleGraph::UUID& id) const {
    auto it = _graph.find_vertex(id);
    if(it == _graph.end()) {
        throw std::out_of_range("No vertex exists for this id");
    }
    return {it->first, it->second.content(), it->second.edges()};
}

bool SimpleGraph::hasVertex(const SimpleGraph::UUID& id) const {
    return _graph.has_vertex(id);
}

bool SimpleGraph::hasEdge(const SimpleGraph::UUID& from,
                          const SimpleGraph::UUID& to) const {
    return _graph.has_edge(from, to);
}


// -----------------------------------------------------------------------------
// Modifiers methods (End-user)
// -----------------------------------------------------------------------------

void SimpleGraph::addVertex(const UUID& id) {
    Timestamp::setEffectiveID(_localID);
    VertexAddOperation op = {id, Timestamp::now()};
    this->applyOperation(op);
    this->notifyOperationBroadcaster(op);
}

void SimpleGraph::removeVertex(const UUID& id) {
    Timestamp::setEffectiveID(_localID);
    VertexRemoveOperation op = {id, Timestamp::now()};
    this->applyOperation(op);
    this->notifyOperationBroadcaster(op);
}

void SimpleGraph::addEdge(const UUID& from, const UUID& to) {
    Timestamp::setEffectiveID(_localID);
    EdgeAddOperation op = {from, to, Timestamp::now()};
    this->applyOperation(op);
    this->notifyOperationBroadcaster(op);
}

void SimpleGraph::removeEdge(const UUID& from, const UUID& to) {
    Timestamp::setEffectiveID(_localID);
    EdgeRemoveOperation op = {from, to, Timestamp::now()};
    this->applyOperation(op);
    this->notifyOperationBroadcaster(op);
}

void SimpleGraph::addAttribute(const UUID& id, const std::string& name,
                               const std::string& value) {
    Timestamp::setEffectiveID(_localID);
    AttributeAddOperation op = {id, Timestamp::now(), name, value};
    this->applyOperation(op);
    this->notifyOperationBroadcaster(op);
}

void SimpleGraph::removeAttribute(const UUID& id, const std::string& name) {
    Timestamp::setEffectiveID(_localID);
    AttributeRemoveOperation op = {id, Timestamp::now(), name};
    this->applyOperation(op);
    this->notifyOperationBroadcaster(op);
}

void SimpleGraph::setAttribute(const UUID& id, const std::string& name,
                               const std::string& value) {
    Timestamp::setEffectiveID(_localID);
    AttributeSetOperation op = {id, Timestamp::now(), name, value};
    this->applyOperation(op);
    this->notifyOperationBroadcaster(op);
}


// -----------------------------------------------------------------------------
// Operation Methods (Internal)
// -----------------------------------------------------------------------------

void SimpleGraph::applyOperation(const VertexAddOperation& op) {
    auto& tnow      = op.timestamp();
    auto& id        = op.vertexID();
    bool isAdded    = _graph.add_vertex(id, tnow);
    if(isAdded) {
        this->notifyOperationObservers(op);
    }
}

void SimpleGraph::applyOperation(const VertexRemoveOperation& op) {
    auto& tnow      = op.timestamp();
    auto& id        = op.vertexID();
    bool isRemoved  = _graph.remove_vertex(id, tnow);
    if(isRemoved) {
        this->notifyOperationObservers(op);
    }
}

void SimpleGraph::applyOperation(const EdgeAddOperation& op) {
    auto& tnow      = op.timestamp();
    auto& from      = op.fromID();
    auto& to        = op.toID();
    CmRDT::AddEdgeInfo info = _graph.add_edge(from, to, tnow);
    if(info.isFromAdded) {
        VertexAddOperation vertex_op = {from, tnow};
        this->notifyOperationObservers(vertex_op);
    }
    if(info.isToAdded) {
        VertexAddOperation vertex_op = {to, tnow};
        this->notifyOperationObservers(vertex_op);
    }
    if(info.isEdgeAdded) {
        this->notifyOperationObservers(op);
    }
}

void SimpleGraph::applyOperation(const EdgeRemoveOperation& op) {
    auto& tnow      = op.timestamp();
    auto& from      = op.fromID();
    auto& to        = op.toID();
    bool isRemoved  = _graph.remove_edge(from, to, tnow);
    if(isRemoved) {
        this->notifyOperationObservers(op);
    }
}

void SimpleGraph::applyOperation(const AttributeAddOperation& op) {
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

void SimpleGraph::applyOperation(const AttributeRemoveOperation& op) {
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

void SimpleGraph::applyOperation(const AttributeSetOperation& op) {
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


// -----------------------------------------------------------------------------
// OperationObserver
// -----------------------------------------------------------------------------

bool SimpleGraph::applyExternOperation(int id, const std::string& buffer) {
    std::stringstream opBuffer(buffer);

    switch(id) {
        case OPERATION_VERTEX_ADD: {
                VertexAddOperation op;
                if(!op.unserialize(opBuffer)) { return false; }
                Timestamp::setEffectiveID(op.timestamp().getID());
                applyOperation(op);
            }
            break;

        case OPERATION_VERTEX_REMOVE: {
                VertexRemoveOperation op;
                if(!op.unserialize(opBuffer)) { return false; }
                Timestamp::setEffectiveID(op.timestamp().getID());
                applyOperation(op);
            }
            break;

        case OPERATION_EDGE_ADD: {
                EdgeAddOperation op;
                if(!op.unserialize(opBuffer)) { return false; }
                Timestamp::setEffectiveID(op.timestamp().getID());
                applyOperation(op);
            }
            break;

        case OPERATION_EDGE_REMOVE: {
                EdgeRemoveOperation op;
                if(!op.unserialize(opBuffer)) { return false; }
                Timestamp::setEffectiveID(op.timestamp().getID());
                applyOperation(op);
            }
            break;

        case OPERATION_ATTRIBUTE_ADD: {
                AttributeAddOperation op;
                if(!op.unserialize(opBuffer)) { return false; }
                Timestamp::setEffectiveID(op.timestamp().getID());
                applyOperation(op);
            }
            break;

        case OPERATION_ATTRIBUTE_REMOVE: {
                AttributeRemoveOperation op;
                if(!op.unserialize(opBuffer)) { return false; }
                Timestamp::setEffectiveID(op.timestamp().getID());
                applyOperation(op);
            }
            break;

        case OPERATION_ATTRIBUTE_SET: {
                AttributeSetOperation op;
                if(!op.unserialize(opBuffer)) { return false; }
                Timestamp::setEffectiveID(op.timestamp().getID());
                applyOperation(op);
            }
            break;

        default:
            return false;
    }

    return true;
}


} // End namespace


