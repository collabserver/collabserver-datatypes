#pragma once

#include "CmRDT/LWWRegister.h"

#include <iostream>

namespace CRDT {
namespace CmRDT {


void LWWRegister_example() {
    std::cout << "\n----- CmRDT LWWRegister Example ----------\n";

    LWWRegister<int, int> data0;
    LWWRegister<int, int> data1;


    // At replicate 0
    data0.update(300, 3);
    std::cout << "update(300, 3) -> " << data0 << "\n";

    data0.update(100, 1);
    std::cout << "update(100, 1) -> " << data0 << "\n";

    data0.update(500, 5);
    std::cout << "update(500, 5) -> " << data0 << "\n";

    data0.update(700, 7);
    std::cout << "update(700, 7) -> " << data0 << "\n";

    data0.update(200, 2);
    std::cout << "update(200, 2) -> " << data0 << "\n";


    // At replicate 1
    data1.update(300, 3);
    data1.update(100, 1);
    data1.update(500, 5);
    data1.update(700, 7);


    // --- Final ---
    std::cout << "(data0 == data1) = " << (data0 == data1);
    std::cout << " (true=" << true << ")\n";
    std::cout << "(data0 != data1) = " << (data0 != data1) << "\n";
}


}} // End namespace


