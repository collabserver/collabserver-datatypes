#pragma once

#include <iostream>
#include <string>

#include "collabserver/data/CmRDT/LWWGraph.h"

namespace collabserver {
namespace data {
namespace CmRDT {

void LWWGraph_example() {
    std::cout << "\n----- CmRDT LWWGraph Example ----------\n";

    LWWGraph<std::string, int, int> data0;
    LWWGraph<std::string, int, int> data1;

    // User 0 creates 2 vertex and link v2 -> v1
    data0.add_vertex("v1", 110);
    data0.add_vertex("v2", 120);
    data0.add_edge("v1", "v2", 130);

    // User 1 creates 2 vertex and link v2 -> v1
    data1.add_vertex("v1", 111);
    data1.add_vertex("v2", 121);
    data1.add_edge("v1", "v2", 131);

    // Broadcast changes from 0 to 1
    data1.add_vertex("v1", 110);
    data1.add_vertex("v2", 120);
    data1.add_edge("v1", "v2", 130);

    // Broadcast changes from 1 to 0
    data0.add_vertex("v1", 111);
    data0.add_vertex("v2", 121);
    data0.add_edge("v1", "v2", 131);

    // At this point, user0 and user1 have actually the same graph.

    // User0 add vertex v3 + edge v1 -> v3
    data0.add_edge("v1", "v3", 140);

    // User1 remove vertex v1
    data1.remove_vertex("v1", 151);

    // Broadcast changes
    data1.add_edge("v1", "v3", 140);
    data0.remove_vertex("v1", 151);

    // At this point, user0 and user1 have the same graph.
    // remove_vertex was later, so all edges with v2 are removed.

    // Final state
    std::cout << "data0 (at the end): " << data0 << "\n";
    std::cout << "data1 (at the end): " << data1 << "\n";

    std::cout << "(data0 == data1) = " << (data0 == data1);
    std::cout << " (true=" << true << ")\n";
    std::cout << "(data0 != data1) = " << (data0 != data1) << "\n";
}

}  // namespace CmRDT
}  // namespace data
}  // namespace collabserver
