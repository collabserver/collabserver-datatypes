#pragma once

#include "CvRDT/GCounter.h"

namespace CRDT {
namespace CvRDT {


void GCounter_example() {
    std::cout << "\n----- CvRDT GCounter Example ----------\n";

    GCounter<int, std::string> data0("user1");
    GCounter<int, std::string> data1("user2");


    // --- Replicate 0 (data0) ---
    data0.increment();
    data0.increment();
    data0.increment();


    // --- Replicate 1 (data1) ---
    data1.increment();
    data1.increment();


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
