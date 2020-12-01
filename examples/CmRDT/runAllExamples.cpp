#include "Example_LWWGraph.h"
#include "Example_LWWMap.h"
#include "Example_LWWSet.h"

using namespace collabserver::data;

// Start all CmRDT examples (Operation-based)
int main(int argc, char** argv) {
    CmRDT::LWWSet_example();
    CmRDT::LWWMap_example();
    CmRDT::LWWGraph_example();

    return 0;
}
