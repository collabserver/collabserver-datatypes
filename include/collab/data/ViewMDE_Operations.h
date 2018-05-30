#pragma once

#include "Timestamp.h"
#include "Operation.h"
#include <string>

class ViewMDE;

namespace collab {


class AddElementOperation : public Operation {
    private:
        friend ViewMDE;
        std::string     _elementID;
        Timestamp       _timestamp;

    public:
        AddElementOperation(const std::string& id, const Timestamp& time)
            : _elementID(id), _timestamp(time) {
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
};


} // End namespace


