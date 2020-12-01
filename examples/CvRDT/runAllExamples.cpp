#include "Example_2PSet.h"
#include "Example_GCounter.h"
#include "Example_GGraph.h"
#include "Example_GMap.h"
#include "Example_GSet.h"
#include "Example_LWWRegister.h"
#include "Example_PNCounter.h"

using namespace collab;

// Start all CvRDT examples (State-based)
int main(int argc, char** argv) {
    CvRDT::GCounter_example();
    CvRDT::PNCounter_example();

    CvRDT::GSet_example();
    CvRDT::TwoPSet_example();

    CvRDT::LWWRegister_example();

    CvRDT::GMap_example();

    CvRDT::GGraph_example();

    return 0;
}
