#include "Example_LWWMap.h"
#include "Example_LWWSet.h"

using namespace collab;


// Start all CmRDT examples (Operation-based)
int main(int argc, char** argv) {

    CmRDT::LWWMap_example();
    CmRDT::LWWSet_example();

    return 0;
}
