#pragma once

#include <string>

#include "Timestamp.h"
#include "Operation.h"

class ViewMDE;

namespace collab {


    /*
class AddElementOperation : public Operation {
    private:
        friend ViewMDE;
        std::string     _elementID;
        Timestamp       _timestamp;

    public:
        AddElementOperation(const std::string& id, const Timestamp& time)
            : _elementID(id), _timestamp(time) {
        }

    public:
        bool serialize(std::stringstream& buffer) const {
            return false;
        }
        bool unserialize(std::stringstream& buffer) {
            return false;
        }
};

class AddAttributeOperation : public Operation {
    private:
        friend ViewMDE;
        std::string     _elementID;
        Timestamp       _timestamp;
        std::string     _name;
        std::string     _value;
        Timestamp       _timestampPrevious;
        std::string     _namePrevious;
        std::string     _valuePrevious;

    public:
        bool serialize(std::stringstream& buffer) const {
            return false;
        }
        bool unserialize(std::stringstream& buffer) {
            return false;
        }
};

*/

} // End namespace


