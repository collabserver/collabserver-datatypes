#pragma once

#include "collabdata/CvRDT/GGraph.h"

#include <iostream>
#include <string>

namespace collab {
namespace CvRDT {


void GGraph_example() {
    std::cout << "\n----- CvRDT GGraph Example ----------\n";

    GGraph<std::string, int> data0;
    GGraph<std::string, int> data1;


    // --- Replicate 0 (data0) ---
    data0.addVertex("v1", 10);
    data0.addVertex("v2", 200);
    data0.addVertex("v3", 30);
    data0.addVertex("v4", 400);

    data0.addEdge("v1", "v2");
    data0.addEdge("v1", "v4");
    data0.addEdge("v2", "v3");
    data0.addEdge("v4", "v3");


    // --- Replicate 1 (data1) ---
    data1.addVertex("v1", 100);
    data1.addVertex("v3", 300);
    data1.addVertex("v4", 40);

    data1.addEdge("v1", "v3");
    data1.addEdge("v3", "v1");
    data1.addEdge("v4", "v1");
    data1.addEdge("v4", "v3");

    // Do nothing because v2 doesn't exists
    data1.addEdge("v1", "v2");
    data1.addEdge("v4", "v2");

    // --- Final ---
    std::cout << "data0 before merge: " << data0 << "\n";
    std::cout << "data1 before merge: " << data1 << "\n";

    data0.merge(data1);
    data1.merge(data0);

    std::cout << "data0 after merge: " << data0 << "\n";
    std::cout << "data1 after merge: " << data1 << "\n";

    std::cout << "(data0 == data1) = " << (data0 == data1);
    std::cout << " (true=" << true << ")\n";
    std::cout << "(data0 != data1) = " << (data0 != data1) << "\n";
}


}} // End namespaces
