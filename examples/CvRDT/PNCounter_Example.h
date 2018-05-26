#pragma once

#include "collab/CvRDT/PNCounter.h"

namespace collab {
namespace CvRDT {


void PNCounter_example() {
    std::cout << "\n----- CvRDT PNCounter Example ----------\n";

    PNCounter<int, std::string> data0("user1");
    PNCounter<int, std::string> data1("user2");


    // --- Replicate 0 (data0) ---
    data0.increment();
    data0.increment();
    data0.increment();
    data0.decrement();
    data0.increment(4);
    data0.decrement(4);
    // = 2


    // --- Replicate 1 (data1) ---
    data1.increment();
    data1.increment();
    data1.decrement();
    data1.decrement();
    data1.decrement();
    // = -1


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
