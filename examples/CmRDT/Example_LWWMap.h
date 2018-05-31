#pragma once

#include "collab/CmRDT/LWWMap.h"

#include <iostream>
#include <string>

namespace collab {
namespace CmRDT {


void LWWMap_example() {
    std::cout << "\n----- CmRDT LWWMap Example ----------\n";

    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // Initial replicate 0 operations
    data0.add("e1", 110);
    data0.remove("e1", 120);
    data0.add("e1", 130);
    data0.add("e2", 140);
    data0.add("e3", 150);
    data0.add("e4", 160);
    data0.add("e5", 170);
    data0.remove("e5", 180);
    data0.remove("e3", 190);


    // Initial replicate 1 operations
    data1.add("e1", 111);
    data1.add("e7", 121);
    data1.add("e8", 131);
    data1.remove("e8", 141);
    data1.remove("e1", 151);


    // Broadcast operations from data1 to data0
    data0.add("e1", 111);
    data0.add("e7", 121);
    data0.add("e8", 131);
    data0.remove("e8", 141);
    data0.remove("e1", 151);


    // Broadcast operations from data0 to data1
    data1.add("e1", 110);
    data1.remove("e1", 120);
    data1.add("e1", 130);
    data1.add("e2", 140);
    data1.add("e3", 150);
    data1.add("e4", 160);
    data1.add("e5", 170);
    data1.remove("e5", 180);
    data1.remove("e3", 190);


    // Final state
    std::cout << "data0 (at the end): " << data0 << "\n";
    std::cout << "data1 (at the end): " << data1 << "\n";

    std::cout << "(data0 == data1) = " << (data0 == data1);
    std::cout << " (true=" << true << ")\n";
    std::cout << "(data0 != data1) = " << (data0 != data1) << "\n";
}

}} // End namespace


