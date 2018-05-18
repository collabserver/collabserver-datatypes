#include "CvRDT/GCounter_Example.h"
#include "CvRDT/PNCounter_Example.h"
#include "CvRDT/LWWRegister_Example.h"
#include "CvRDT/GSet_Example.h"
#include "CvRDT/2PSet_Example.h"
#include "CvRDT/GMap_Example.h"
#include "CvRDT/GGraph_Example.h"

using namespace CRDT;


int main(int argc, char** argv) {

    // CvRDT (State-based)

    CvRDT::GCounter_example();
    CvRDT::PNCounter_example();

    CvRDT::GSet_example();
    CvRDT::TwoPSet_example();

    CvRDT::LWWRegister_example();

    CvRDT::GMap_example();

    CvRDT::GGraph_example();
    return 0;
}
