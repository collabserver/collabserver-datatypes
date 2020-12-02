#pragma once

namespace collabserver {

/**
 * \brief
 * Interface for a Handler of CollabDataOperation.
 *
 * This class is only an interface to use in concrete CollabData.
 * Therefore, no methods are given. This is based on a visitor pattern.
 *
 *
 * \par Example
 * \code{.cpp}
 * #include <iostream>
 * namespace collabserver {
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
 * class ObserverExample : public CollabDataOperationObserver {
 *     private:
 *         HandlerExample _handler;
 *     public:
 *         void onOperation(const CollabDataOperation& op) override {
 *             op.accept(_handler);
 *         }
 * };
 *
 * } // End namespace
 * \endcode
 *
 *
 * \see CollabData
 * \see CollabDataOperation
 * \see CollabDataOperationObserver
 */
class CollabDataOperationHandler {
   protected:
    CollabDataOperationHandler() = default;
    CollabDataOperationHandler(const CollabDataOperationHandler& other) = default;
    CollabDataOperationHandler& operator=(const CollabDataOperationHandler& other) = default;
};

}  // namespace collabserver
