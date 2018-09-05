#include "collabdata/custom/SimpleGraph.h"

#include <msgpack.hpp>

#include "collabdata/custom/Timestamp.h"

namespace collab {


// TODO DEV NOTE
// For now, the serialization process is really naive and insecure
// True is always returned: I should add validation checking!


// -----------------------------------------------------------------------------
// VertexAddOperation
// -----------------------------------------------------------------------------

SimpleGraph::VertexAddOperation::VertexAddOperation(
        const std::string& id,
        const Timestamp& time)
        : _vertexID(id), _timestamp(time) {
}

bool SimpleGraph::VertexAddOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, time.count());

    return true;
}

bool SimpleGraph::VertexAddOperation::unserialize(const std::stringstream& buffer) {
    const char* data    = buffer.str().data();
    const size_t size   = buffer.str().size();
    std::size_t off     = 0;

    msgpack::unpacked r1;
    msgpack::unpacked r2;

    msgpack::unpack(r1, data, size, off);
    msgpack::unpack(r2, data, size, off);

    r1.get().convert(_vertexID);
    auto time = r2.get().as<Timestamp::Clock::rep>();

    Timestamp::Clock::duration ellie = Timestamp::Clock::duration(time);
    _timestamp = Timestamp(Timestamp::TimePoint(ellie));

    return true;
}

void SimpleGraph::VertexAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}


// -----------------------------------------------------------------------------
// VertexRemoveOperation
// -----------------------------------------------------------------------------

SimpleGraph::VertexRemoveOperation::VertexRemoveOperation(
        const std::string& id,
        const Timestamp& time)
        : _vertexID(id), _timestamp(time) {
}

bool SimpleGraph::VertexRemoveOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, time.count());

    return true;
}

bool SimpleGraph::VertexRemoveOperation::unserialize(const std::stringstream& buffer) {
    const char* data    = buffer.str().data();
    const size_t size   = buffer.str().size();
    std::size_t off     = 0;

    msgpack::unpacked r1;
    msgpack::unpacked r2;

    msgpack::unpack(r1, data, size, off);
    msgpack::unpack(r2, data, size, off);

    r1.get().convert(_vertexID);
    auto time = r2.get().as<Timestamp::Clock::rep>();

    Timestamp::Clock::duration ellie = Timestamp::Clock::duration(time);
    _timestamp = Timestamp(Timestamp::TimePoint(ellie));

    return true;
}

void SimpleGraph::VertexRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
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

bool SimpleGraph::EdgeAddOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _fromID);
    msgpack::pack(buffer, _toID);
    msgpack::pack(buffer, time.count());

    return true;
}

bool SimpleGraph::EdgeAddOperation::unserialize(const std::stringstream& buffer) {
    const char* data    = buffer.str().data();
    const size_t size   = buffer.str().size();
    std::size_t off     = 0;

    msgpack::unpacked r1;
    msgpack::unpacked r2;
    msgpack::unpacked r3;

    msgpack::unpack(r1, data, size, off);
    msgpack::unpack(r2, data, size, off);
    msgpack::unpack(r3, data, size, off);

    r1.get().convert(_fromID);
    r2.get().convert(_toID);
    auto time = r3.get().as<Timestamp::Clock::rep>();

    Timestamp::Clock::duration ellie = Timestamp::Clock::duration(time);
    _timestamp = Timestamp(Timestamp::TimePoint(ellie));

    return true;
}

void SimpleGraph::EdgeAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
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

bool SimpleGraph::EdgeRemoveOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _fromID);
    msgpack::pack(buffer, _toID);
    msgpack::pack(buffer, time.count());

    return true;
}
bool SimpleGraph::EdgeRemoveOperation::unserialize(const std::stringstream& buffer) {
    const char* data    = buffer.str().data();
    const size_t size   = buffer.str().size();
    std::size_t off     = 0;

    msgpack::unpacked r1;
    msgpack::unpacked r2;
    msgpack::unpacked r3;

    msgpack::unpack(r1, data, size, off);
    msgpack::unpack(r2, data, size, off);
    msgpack::unpack(r3, data, size, off);

    r1.get().convert(_fromID);
    r2.get().convert(_toID);
    auto time = r3.get().as<Timestamp::Clock::rep>();

    Timestamp::Clock::duration ellie = Timestamp::Clock::duration(time);
    _timestamp = Timestamp(Timestamp::TimePoint(ellie));

    return true;
}

void SimpleGraph::EdgeRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
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

bool SimpleGraph::AttributeAddOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, time.count());
    msgpack::pack(buffer, _attributeName);
    msgpack::pack(buffer, _attributeValue);

    return true;
}

bool SimpleGraph::AttributeAddOperation::unserialize(const std::stringstream& buffer) {
    const char* data    = buffer.str().data();
    const size_t size   = buffer.str().size();
    std::size_t off     = 0;

    msgpack::unpacked r1;
    msgpack::unpacked r2;
    msgpack::unpacked r3;
    msgpack::unpacked r4;

    msgpack::unpack(r1, data, size, off);
    msgpack::unpack(r2, data, size, off);
    msgpack::unpack(r3, data, size, off);
    msgpack::unpack(r4, data, size, off);

    r1.get().convert(_vertexID);
    auto time = r2.get().as<Timestamp::Clock::rep>();
    r3.get().convert(_attributeName);
    r4.get().convert(_attributeValue);

    Timestamp::Clock::duration ellie = Timestamp::Clock::duration(time);
    _timestamp = Timestamp(Timestamp::TimePoint(ellie));

    return true;
}

void SimpleGraph::AttributeAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
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

bool SimpleGraph::AttributeRemoveOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, time.count());
    msgpack::pack(buffer, _attributeName);

    return true;
}

bool SimpleGraph::AttributeRemoveOperation::unserialize(const std::stringstream& buffer) {
    const char* data    = buffer.str().data();
    const size_t size   = buffer.str().size();
    std::size_t off     = 0;

    msgpack::unpacked r1;
    msgpack::unpacked r2;
    msgpack::unpacked r3;

    msgpack::unpack(r1, data, size, off);
    msgpack::unpack(r2, data, size, off);
    msgpack::unpack(r3, data, size, off);

    r1.get().convert(_vertexID);
    auto time = r2.get().as<Timestamp::Clock::rep>();
    r3.get().convert(_attributeName);

    Timestamp::Clock::duration ellie = Timestamp::Clock::duration(time);
    _timestamp = Timestamp(Timestamp::TimePoint(ellie));

    return true;
}

void SimpleGraph::AttributeRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
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

bool SimpleGraph::AttributeSetOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, time.count());
    msgpack::pack(buffer, _attributeName);
    msgpack::pack(buffer, _newValue);

    return true;
}
bool SimpleGraph::AttributeSetOperation::unserialize(const std::stringstream& buffer) {
    const char* data    = buffer.str().data();
    const size_t size   = buffer.str().size();
    std::size_t off     = 0;

    msgpack::unpacked r1;
    msgpack::unpacked r2;
    msgpack::unpacked r3;
    msgpack::unpacked r4;

    msgpack::unpack(r1, data, size, off);
    msgpack::unpack(r2, data, size, off);
    msgpack::unpack(r3, data, size, off);
    msgpack::unpack(r4, data, size, off);

    r1.get().convert(_vertexID);
    auto time = r2.get().as<Timestamp::Clock::rep>();
    r3.get().convert(_attributeName);
    r4.get().convert(_newValue);

    Timestamp::Clock::duration ellie = Timestamp::Clock::duration(time);
    _timestamp = Timestamp(Timestamp::TimePoint(ellie));

    return true;
}

void SimpleGraph::AttributeSetOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}


} // End namespace


