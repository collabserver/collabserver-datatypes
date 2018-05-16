#pragma once

#include "CmRDT/LWWSet.h"

#include <iostream>
#include <string>

namespace CRDT {
namespace CmRDT {


void LWWSet_example() {
    std::cout << "\n----- CmRDT LWWSet Example ----------\n";

    LWWSet<std::string, int> data0; // Data at replicate 0
    LWWSet<std::string, int> data1; // Data at replicate 1


    // User1 flow (Normal order)
    data0.add("v1", 1);
    data0.add("v2", 3);
    data0.remove("v1", 4);

    // User1 flow
    data1.remove("v1", 4);
    data1.add("v2", 3);
    data1.add("v1", 1);

    // --- Final ---
    std::cout << "data0 = " << data0 << "\n";
    std::cout << "data1 = " << data1 << "\n";

    std::cout << "(data0 == data1) = " << (data0 == data1);
    std::cout << " (true=" << true << ")\n";
    std::cout << "(data0 != data1) = " << (data0 != data1) << "\n";
}


}} // End namespaces


