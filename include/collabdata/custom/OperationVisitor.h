#pragma once

namespace collab {

class Operation;


class OperationVisitor {
    protected:
        OperationVisitor() = default;
        OperationVisitor(const OperationVisitor& other) = default;
        OperationVisitor& operator=(const OperationVisitor& other) = default;

    public:
        virtual void visit(const Operation& op) = 0;

};


} // End namespace


