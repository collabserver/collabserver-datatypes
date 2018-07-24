#include "collabdata/custom/SimpleGraph.h"

namespace collab {


// -----------------------------------------------------------------------------
// VertexIterator
// -----------------------------------------------------------------------------

SimpleGraph::VertexDescriptor SimpleGraph::VertexIterator::current() const {
    auto& current = *_it;
    return {current.first, current.second.content(), current.second.edges()};
}

bool SimpleGraph::VertexIterator::moveNext() {
    if(_reset) {
        _reset = false;
        return _it != _data.cend();
    }
    ++_it;
    return _it != _data.cend();
}


// -----------------------------------------------------------------------------
// EdgeIterator
// -----------------------------------------------------------------------------

const SimpleGraph::UUID& SimpleGraph::EdgeIterator::current() const {
    return *_it;
}

bool SimpleGraph::EdgeIterator::moveNext() {
    if(_reset) {
        _reset = false;
        return _it != _data.cend();
    }
    ++_it;
    return _it != _data.cend();
}


// -----------------------------------------------------------------------------
// AttributeIterator
// -----------------------------------------------------------------------------

const SimpleGraph::AttributeDescriptor SimpleGraph::AttributeIterator::current() const {
    auto& current = *_it;
    return {current.first, current.second.query()};
}

bool SimpleGraph::AttributeIterator::moveNext() {
    if(_reset) {
        _reset = false;
        return _it != _data.cend();
    }
    ++_it;
    return _it != _data.cend();
}


// -----------------------------------------------------------------------------
// VertexDescriptor
// -----------------------------------------------------------------------------

SimpleGraph::VertexDescriptor::VertexDescriptor(
        const SimpleGraph::UUID& id,
        const _AttributeMap& attributes,
        const _EdgeSet& edges)
    : _id(id),
      _content(attributes),
      _edges(edges) {
}

const SimpleGraph::UUID& SimpleGraph::VertexDescriptor::id() const {
    return _id;
}

SimpleGraph::EdgeIterator SimpleGraph::VertexDescriptor::edges() const {
    return EdgeIterator(_edges);
}

SimpleGraph::AttributeIterator SimpleGraph::VertexDescriptor::attributes() const {
    return AttributeIterator(_content);
}


} // End namespace


