#pragma once

namespace collab {

class Operation;


/**
 * \brief
 * Interface for a Handler of Operation.
 *
 * This class is only an interface to use in concrete CollabData.
 * Therefore, no methods are given. This is based on a visitor pattern.
 *
 *
 * \par Example
 * \code{.cpp}
 * #include <iostream>
 * namespace collab {
 *
 * class HandlerExample : public YourData::OpHandler {
 *     public:
 *         void handleOperationX(const YourData::OpX& op) {
 *             std::cout << "This is operation X\n";
 *         }
 *         void handleOperationY(const YourData::OpY& op) {
 *             std::cout << "This is operation Y\n";
 *         }
 * };
 *
 * class ObserverExample : public OperationObserver {
 *     private:
 *         HandlerExample _handler;
 *     public:
 *         void onOperation(const Operation& op) override {
 *             op.accept(_handler);
 *         }
 * };
 *
 * } // End namespace
 * \endcode
 *
 *
 * \see CollabData
 * \see Operation
 * \see OperationObserver
 */
class OperationHandler {
    protected:
        OperationHandler() = default;
        OperationHandler(const OperationHandler& other) = default;
        OperationHandler& operator=(const OperationHandler& other) = default;
};


} // End namespace


