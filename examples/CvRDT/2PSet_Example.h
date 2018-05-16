#pragma once

#include "CvRDT/2PSet.h"

#include <iostream>

namespace CRDT {
namespace CvRDT {


void TwoPSet_example() {
    std::cout << "\n----- CvRDT 2PSet Example ----------\n";

    TwoPSet<int> data0; // Ex: data at replicate 0
    TwoPSet<int> data1; // Ex: data at replicate 1


    // --- Replicate 0 (data0) ---

    // Erase called before insert. But must commute so 1 can't be added.
    data0.erase(1);

    // clear: Removes previous added and can't be re-added
    data0.insert(11);
    data0.insert(12);
    data0.insert(13);
    data0.clear();

    data0.insert(0);
    data0.insert(1);
    data0.insert(2);
    data0.insert(3);
    data0.insert(4);

    data0.erase(0);
    data0.erase(3);

    // Erase idempotent
    data0.erase(0);
    data0.erase(4);

    // Re-insert do nothing
    data0.insert(0);
    data0.insert(3);
    // data0 = {2}


    // --- Replicate 1 (data1) ---
    data1.insert(0);
    data1.clear();

    data1.insert(2);
    data1.insert(4);
    data1.insert(5);
    data1.insert(6);

    data1.insert(11);
    data1.insert(12);
    data1.insert(13);

    data1.erase(0);
    data1.erase(4);
    // data1 = {2,5,6,11,12,13}


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
