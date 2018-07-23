#include "collabdata/custom/SimpleGraph.h"

#include <iostream>

namespace collab {


// Example of end-user handler for each operation on SimpleGraph
class SimpleGraphHandler : public SimpleGraph::OpHandler {
    public:
        void handleOperation(const SimpleGraph::VertexAddOperation& op) {
            std::cout << "handleOperation(VertexAddOperation)\n";
        }
        void handleOperation(const SimpleGraph::VertexRemoveOperation& op) {
            std::cout << "handleOperation(VertexRemoveOperation)\n";
        }
        void handleOperation(const SimpleGraph::EdgeAddOperation& op) {
            std::cout << "handleOperation(EdgeAddOperation)\n";
        }
        void handleOperation(const SimpleGraph::EdgeRemoveOperation& op) {
            std::cout << "handleOperation(EdgeRemoveOperation)\n";
        }
        void handleOperation(const SimpleGraph::AttributeAddOperation& op) {
            std::cout << "handleOperation(AttributeAddOperation)\n";
        }
        void handleOperation(const SimpleGraph::AttributeRemoveOperation& op) {
            std::cout << "handleOperation(AttributeRemoveOperation)\n";
        }
        void handleOperation(const SimpleGraph::AttributeSetOperation& op) {
            std::cout << "handleOperation(AttributeSetOperation)\n";
        }
};


// Example of SimpleGraph Observer
class SimpleGraphObserver : public OperationObserver {
    private:
        SimpleGraphHandler _handler;
    public:
        void onOperation(const Operation& op) override {
            op.accept(_handler);
        }
};


void SimpleGraph_example() {
    std::cout << "\n----- SimpleGraph Example ----------\n";

    // Init workflow
    SimpleGraph data0;
    SimpleGraphObserver observer;
    data0.addOperationObserver(observer);

    data0.addVertex("v1");
    data0.addVertex("v2");
    data0.addEdge("v1", "v2");

    // Setup v1
    std::cout << "--- Setup v1 ---\n";

    data0.addAttribute("v1", "name", "Sylvanas");
    data0.addAttribute("v1", "game", "Wow");
    data0.addAttribute("v1", "faction", "Horde");
    data0.addAttribute("v1", "food", "Who knows?");
    data0.setAttribute("v1", "food", "Noodles");
    data0.removeAttribute("v1", "food");

    // Setup v2
    std::cout << "--- Setup v2 ---\n";
    data0.addAttribute("v2", "name", "Tracer");
    data0.setAttribute("v2", "friend", "widow");
    data0.setAttribute("v2", "friend", "Winston");
    data0.addAttribute("v2", "enemy", "widow");
}


} // End namespace


