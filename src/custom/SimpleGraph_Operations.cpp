#include "collabdata/custom/SimpleGraph.h"

namespace collab {


// -----------------------------------------------------------------------------
// VertexAddOperation
// -----------------------------------------------------------------------------

SimpleGraph::VertexAddOperation::VertexAddOperation(
        const std::string& id,
        const Timestamp& time)
        : _vertexID(id), _timestamp(time) {
}

int SimpleGraph::VertexAddOperation::getType() const {
    return OPERATION_VERTEX_ADD;
}

bool SimpleGraph::VertexAddOperation::serialize(std::stringstream& buffer) const {
    // TODO
    return false;
}

bool SimpleGraph::VertexAddOperation::unserialize(const std::stringstream& buffer) {
    // TODO
    return false;
}

void SimpleGraph::VertexAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

const SimpleGraph::UUID& SimpleGraph::VertexAddOperation::vertexID() const {
    return _vertexID;
}

const Timestamp& SimpleGraph::VertexAddOperation::timestamp() const {
    return _timestamp;
}


// -----------------------------------------------------------------------------
// VertexRemoveOperation
// -----------------------------------------------------------------------------

SimpleGraph::VertexRemoveOperation::VertexRemoveOperation(
        const std::string& id,
        const Timestamp& time)
        : _vertexID(id), _timestamp(time) {
}

int SimpleGraph::VertexRemoveOperation::getType() const {
    return OPERATION_VERTEX_REMOVE;
}

bool SimpleGraph::VertexRemoveOperation::serialize(std::stringstream& buffer) const {
    // TODO
    return false;
}

bool SimpleGraph::VertexRemoveOperation::unserialize(const std::stringstream& buffer) {
    // TODO
    return false;
}

void SimpleGraph::VertexRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

const SimpleGraph::UUID& SimpleGraph::VertexRemoveOperation::vertexID() const {
    return _vertexID;
}

const Timestamp& SimpleGraph::VertexRemoveOperation::timestamp() const {
    return _timestamp;
}


// -----------------------------------------------------------------------------
// EdgeAddOperation
// -----------------------------------------------------------------------------

SimpleGraph::EdgeAddOperation::EdgeAddOperation(
        const UUID& fromID,
        const UUID& toID,
        const Timestamp& time)
        : _fromID(fromID),
          _toID(toID),
          _timestamp(time) {
}

int SimpleGraph::EdgeAddOperation::getType() const {
    return OPERATION_EDGE_ADD;
}

bool SimpleGraph::EdgeAddOperation::serialize(std::stringstream& buffer) const {
    // TODO
    return false;
}

bool SimpleGraph::EdgeAddOperation::unserialize(const std::stringstream& buffer) {
    // TODO
    return false;
}

void SimpleGraph::EdgeAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

const SimpleGraph::UUID& SimpleGraph::EdgeAddOperation::fromID() const {
    return _fromID;
}

const SimpleGraph::UUID& SimpleGraph::EdgeAddOperation::toID() const {
    return _toID;
}

const Timestamp& SimpleGraph::EdgeAddOperation::timestamp() const {
    return _timestamp;
}


// -----------------------------------------------------------------------------
// EdgeRemoveOperation
// -----------------------------------------------------------------------------

SimpleGraph::EdgeRemoveOperation::EdgeRemoveOperation(
        const UUID& fromID,
        const UUID& toID,
        const Timestamp& time)
        : _fromID(fromID), _toID(toID), _timestamp(time) {
}

int SimpleGraph::EdgeRemoveOperation::getType() const {
    return OPERATION_EDGE_REMOVE;
}

bool SimpleGraph::EdgeRemoveOperation::serialize(std::stringstream& buffer) const {
    // TODO
    return false;
}
bool SimpleGraph::EdgeRemoveOperation::unserialize(const std::stringstream& buffer) {
    // TODO
    return false;
}

void SimpleGraph::EdgeRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

const SimpleGraph::UUID& SimpleGraph::EdgeRemoveOperation::fromID() const {
    return _fromID;
}

const SimpleGraph::UUID& SimpleGraph::EdgeRemoveOperation::toID() const {
    return _toID;
}

const Timestamp& SimpleGraph::EdgeRemoveOperation::timestamp() const {
    return _timestamp;
}


// -----------------------------------------------------------------------------
// AttributeAddOperation
// -----------------------------------------------------------------------------

SimpleGraph::AttributeAddOperation::AttributeAddOperation(
        const std::string& id,
        const Timestamp& time,
        const std::string& name,
        const std::string& value)
        : _vertexID(id),
          _timestamp(time),
          _attributeName(name),
          _attributeValue(value) {
}

int SimpleGraph::AttributeAddOperation::getType() const {
    return OPERATION_ATTRIBUTE_ADD;
}

bool SimpleGraph::AttributeAddOperation::serialize(std::stringstream& buffer) const {
    // TODO
    return false;
}

bool SimpleGraph::AttributeAddOperation::unserialize(const std::stringstream& buffer) {
    // TODO
    return false;
}

void SimpleGraph::AttributeAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

const SimpleGraph::UUID& SimpleGraph::AttributeAddOperation::vertexID() const {
    return _vertexID;
}

const Timestamp& SimpleGraph::AttributeAddOperation::timestamp() const {
    return _timestamp;
}

const std::string& SimpleGraph::AttributeAddOperation::attributeName() const {
    return _attributeName;
}

const std::string& SimpleGraph::AttributeAddOperation::attributeValue() const {
    return _attributeValue;
}


// -----------------------------------------------------------------------------
// AttributeRemoveOperation
// -----------------------------------------------------------------------------

SimpleGraph::AttributeRemoveOperation::AttributeRemoveOperation(
        const std::string& id,
        const Timestamp& time,
        const std::string& name)
        : _vertexID(id),
          _timestamp(time),
          _attributeName(name) {
}

int SimpleGraph::AttributeRemoveOperation::getType() const {
    return OPERATION_ATTRIBUTE_REMOVE;
}

bool SimpleGraph::AttributeRemoveOperation::serialize(std::stringstream& buffer) const {
    // TODO
    return false;
}

bool SimpleGraph::AttributeRemoveOperation::unserialize(const std::stringstream& buffer) {
    // TODO
    return false;
}

void SimpleGraph::AttributeRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

const SimpleGraph::UUID& SimpleGraph::AttributeRemoveOperation::vertexID() const {
    return _vertexID;
}

const Timestamp& SimpleGraph::AttributeRemoveOperation::timestamp() const {
    return _timestamp;
}

const std::string& SimpleGraph::AttributeRemoveOperation::attributeName() const {
    return _attributeName;
}



// -----------------------------------------------------------------------------
// AttributeSetOperation
// -----------------------------------------------------------------------------

SimpleGraph::AttributeSetOperation::AttributeSetOperation(
        const std::string& id,
        const Timestamp& time,
        const std::string& name,
        const std::string& nVal)
        : _vertexID(id),
          _timestamp(time),
          _attributeName(name),
          _newValue(nVal) {
}

int SimpleGraph::AttributeSetOperation::getType() const {
    return OPERATION_ATTRIBUTE_SET;
}

bool SimpleGraph::AttributeSetOperation::serialize(std::stringstream& buffer) const {
    // TODO
    return false;
}
bool SimpleGraph::AttributeSetOperation::unserialize(const std::stringstream& buffer) {
    // TODO
    return false;
}

void SimpleGraph::AttributeSetOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

const SimpleGraph::UUID& SimpleGraph::AttributeSetOperation::vertexID() const {
    return _vertexID;
}

const Timestamp& SimpleGraph::AttributeSetOperation::timestamp() const {
    return _timestamp;
}

const std::string& SimpleGraph::AttributeSetOperation::attributeName() const {
    return _attributeName;
}

const std::string& SimpleGraph::AttributeSetOperation::newValue() const {
    return _newValue;
}


} // End namespace


