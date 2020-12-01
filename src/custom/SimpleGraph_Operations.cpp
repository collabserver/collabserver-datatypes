#include <cassert>
#include <msgpack.hpp>

#include "collabdata/custom/SimpleGraph.h"
#include "collabdata/custom/Timestamp.h"

// Unpack a Timestamp data (Using msgpack)
#define MACRO_UNPACK_TIMESTAMP(data, size, off)                          \
    msgpack::object_handle _r1 = msgpack::unpack(data, size, off);       \
    msgpack::object_handle _r2 = msgpack::unpack(data, size, off);       \
    unsigned int userID = _r1.get().as<unsigned int>();                  \
    auto time = _r2.get().as<Timestamp::Clock::rep>();                   \
    Timestamp::Clock::duration ellie = Timestamp::Clock::duration(time); \
    _timestamp = Timestamp(Timestamp::TimePoint(ellie), userID)

namespace collab {

// TODO DEV NOTE
// Serialization / unserialization may require more success checking.

// -----------------------------------------------------------------------------
// VertexAddOperation
// -----------------------------------------------------------------------------

SimpleGraph::VertexAddOperation::VertexAddOperation(const std::string& id, const Timestamp& time)
    : _vertexID(id), _timestamp(time) {}

bool SimpleGraph::VertexAddOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, _timestamp.getID());
    msgpack::pack(buffer, time.count());

    return true;
}

bool SimpleGraph::VertexAddOperation::unserialize(const std::stringstream& buffer) {
    std::string str(buffer.str());
    std::size_t off = 0;

    msgpack::object_handle r1 = msgpack::unpack(str.data(), str.size(), off);

    r1.get().convert(_vertexID);

    MACRO_UNPACK_TIMESTAMP(str.data(), str.size(), off);

    assert(off == str.size());
    return off == str.size();
}

void SimpleGraph::VertexAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

// -----------------------------------------------------------------------------
// VertexRemoveOperation
// -----------------------------------------------------------------------------

SimpleGraph::VertexRemoveOperation::VertexRemoveOperation(const std::string& id, const Timestamp& time)
    : _vertexID(id), _timestamp(time) {}

bool SimpleGraph::VertexRemoveOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, _timestamp.getID());
    msgpack::pack(buffer, time.count());

    return true;
}

bool SimpleGraph::VertexRemoveOperation::unserialize(const std::stringstream& buffer) {
    std::string str(buffer.str());
    std::size_t off = 0;

    msgpack::object_handle r1 = msgpack::unpack(str.data(), str.size(), off);

    r1.get().convert(_vertexID);

    MACRO_UNPACK_TIMESTAMP(str.data(), str.size(), off);

    assert(off == str.size());
    return off == str.size();
}

void SimpleGraph::VertexRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

// -----------------------------------------------------------------------------
// EdgeAddOperation
// -----------------------------------------------------------------------------

SimpleGraph::EdgeAddOperation::EdgeAddOperation(const UUID& fromID, const UUID& toID, const Timestamp& time)
    : _fromID(fromID), _toID(toID), _timestamp(time) {}

bool SimpleGraph::EdgeAddOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _fromID);
    msgpack::pack(buffer, _toID);
    msgpack::pack(buffer, _timestamp.getID());
    msgpack::pack(buffer, time.count());

    return true;
}

bool SimpleGraph::EdgeAddOperation::unserialize(const std::stringstream& buffer) {
    std::string str(buffer.str());
    std::size_t off = 0;

    msgpack::object_handle r1 = msgpack::unpack(str.data(), str.size(), off);
    msgpack::object_handle r2 = msgpack::unpack(str.data(), str.size(), off);

    r1.get().convert(_fromID);
    r2.get().convert(_toID);

    MACRO_UNPACK_TIMESTAMP(str.data(), str.size(), off);

    assert(off == str.size());
    return off == str.size();
}

void SimpleGraph::EdgeAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

// -----------------------------------------------------------------------------
// EdgeRemoveOperation
// -----------------------------------------------------------------------------

SimpleGraph::EdgeRemoveOperation::EdgeRemoveOperation(const UUID& fromID, const UUID& toID, const Timestamp& time)
    : _fromID(fromID), _toID(toID), _timestamp(time) {}

bool SimpleGraph::EdgeRemoveOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _fromID);
    msgpack::pack(buffer, _toID);
    msgpack::pack(buffer, _timestamp.getID());
    msgpack::pack(buffer, time.count());

    return true;
}
bool SimpleGraph::EdgeRemoveOperation::unserialize(const std::stringstream& buffer) {
    std::string str(buffer.str());
    std::size_t off = 0;

    msgpack::object_handle r1 = msgpack::unpack(str.data(), str.size(), off);
    msgpack::object_handle r2 = msgpack::unpack(str.data(), str.size(), off);

    r1.get().convert(_fromID);
    r2.get().convert(_toID);

    MACRO_UNPACK_TIMESTAMP(str.data(), str.size(), off);

    assert(off == str.size());
    return off == str.size();
}

void SimpleGraph::EdgeRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

// -----------------------------------------------------------------------------
// AttributeAddOperation
// -----------------------------------------------------------------------------

SimpleGraph::AttributeAddOperation::AttributeAddOperation(const std::string& id, const Timestamp& time,
                                                          const std::string& name, const std::string& value)
    : _vertexID(id), _timestamp(time), _attributeName(name), _attributeValue(value) {}

bool SimpleGraph::AttributeAddOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, _attributeName);
    msgpack::pack(buffer, _attributeValue);
    msgpack::pack(buffer, _timestamp.getID());
    msgpack::pack(buffer, time.count());

    return true;
}

bool SimpleGraph::AttributeAddOperation::unserialize(const std::stringstream& buffer) {
    std::string str(buffer.str());
    std::size_t off = 0;

    msgpack::object_handle r1 = msgpack::unpack(str.data(), str.size(), off);
    msgpack::object_handle r2 = msgpack::unpack(str.data(), str.size(), off);
    msgpack::object_handle r3 = msgpack::unpack(str.data(), str.size(), off);

    r1.get().convert(_vertexID);
    r2.get().convert(_attributeName);
    r3.get().convert(_attributeValue);

    MACRO_UNPACK_TIMESTAMP(str.data(), str.size(), off);

    assert(off == str.size());
    return off == str.size();
}

void SimpleGraph::AttributeAddOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

// -----------------------------------------------------------------------------
// AttributeRemoveOperation
// -----------------------------------------------------------------------------

SimpleGraph::AttributeRemoveOperation::AttributeRemoveOperation(const std::string& id, const Timestamp& time,
                                                                const std::string& name)
    : _vertexID(id), _timestamp(time), _attributeName(name) {}

bool SimpleGraph::AttributeRemoveOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, _attributeName);
    msgpack::pack(buffer, _timestamp.getID());
    msgpack::pack(buffer, time.count());

    return true;
}

bool SimpleGraph::AttributeRemoveOperation::unserialize(const std::stringstream& buffer) {
    std::string str(buffer.str());
    std::size_t off = 0;

    msgpack::object_handle r1 = msgpack::unpack(str.data(), str.size(), off);
    msgpack::object_handle r2 = msgpack::unpack(str.data(), str.size(), off);

    r1.get().convert(_vertexID);
    r2.get().convert(_attributeName);

    MACRO_UNPACK_TIMESTAMP(str.data(), str.size(), off);

    assert(off == str.size());
    return off == str.size();
}

void SimpleGraph::AttributeRemoveOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

// -----------------------------------------------------------------------------
// AttributeSetOperation
// -----------------------------------------------------------------------------

SimpleGraph::AttributeSetOperation::AttributeSetOperation(const std::string& id, const Timestamp& time,
                                                          const std::string& name, const std::string& nVal)
    : _vertexID(id), _timestamp(time), _attributeName(name), _attributeValue(nVal) {}

bool SimpleGraph::AttributeSetOperation::serialize(std::stringstream& buffer) const {
    Timestamp::Clock::duration time = _timestamp.getTime().time_since_epoch();

    msgpack::pack(buffer, _vertexID);
    msgpack::pack(buffer, _attributeName);
    msgpack::pack(buffer, _attributeValue);
    msgpack::pack(buffer, _timestamp.getID());
    msgpack::pack(buffer, time.count());

    return true;
}
bool SimpleGraph::AttributeSetOperation::unserialize(const std::stringstream& buffer) {
    std::string str(buffer.str());
    std::size_t off = 0;

    msgpack::object_handle r1 = msgpack::unpack(str.data(), str.size(), off);
    msgpack::object_handle r2 = msgpack::unpack(str.data(), str.size(), off);
    msgpack::object_handle r3 = msgpack::unpack(str.data(), str.size(), off);

    r1.get().convert(_vertexID);
    r2.get().convert(_attributeName);
    r3.get().convert(_attributeValue);

    MACRO_UNPACK_TIMESTAMP(str.data(), str.size(), off);

    assert(off == str.size());
    return off == str.size();
}

void SimpleGraph::AttributeSetOperation::accept(OperationHandler& handler) const {
    static_cast<SimpleGraph::OpHandler&>(handler).handleOperation(*this);
}

}  // namespace collab
