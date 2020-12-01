#pragma once

#include <iostream>
#include <string>

#include "collabdata/CvRDT/GMap.h"

namespace collab {
namespace CvRDT {

void GMap_example() {
    std::cout << "\n----- CvRDT GMap Example ----------\n";

    GMap<std::string, int> data0;  // Data at replicate 0
    GMap<std::string, int> data1;  // Data at replicate 1

    // --- Replicate 0 (data0) ---
    // [ (v1=100), (v2=20), (v3=300), (v4=400), (v9=900) ]
    data0.insert({"v1", 100});
    data0.insert({"v2", 20});
    data0.insert({"v3", 300});
    data0.insert({"v4", 4444});
    data0["v4"] = 400;  // Replace old value
    data0["v9"] = 900;
    data0.insert({"v9", 9999});  // No nothing because already in map

    // --- Replicate 1 (data1) ---
    // [ (v1=0), (v2=200), (v3=30), (v5=500), (v6=600) ]
    data1.insert({"v1", 0});
    data1.insert({"v2", 200});
    data1.insert({"v3", 30});
    data1.insert({"v5", 500});
    data1.insert({"v6", 600});

    // --- Final ---
    // [ all xxx numbers ]
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

}  // namespace CvRDT
}  // namespace collab
