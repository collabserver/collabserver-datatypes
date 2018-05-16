#pragma once

#include "CvRDT/GSet.h"

namespace CRDT {
namespace CvRDT {


void GSet_example() {
    std::cout << "\n----- CvRDT GSet Example ----------\n";

    GSet<int> data0; // Data at replicate 0
    GSet<int> data1; // Data at replicate 1


    // --- Replicate 0 (data0) ---
    // data0 = [0,2,4]
    data0.insert(0);
    data0.insert(2);
    data0.insert(4);


    // --- Replicate 1 (data1) ---
    // data1 = [1,2,3,4,5]
    data1.insert(1);
    data1.insert(2);
    data1.insert(3);
    data1.insert(4);
    data1.insert(5);


    // --- Final ---
    // Merge result = {0,1,2,3,4,5}
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
