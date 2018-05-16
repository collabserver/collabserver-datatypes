// CvRDT (State-based)
#include "CvRDT/GCounter_Example.h"
#include "CvRDT/PNCounter_Example.h"
#include "CvRDT/LWWRegister_Example.h"
#include "CvRDT/GSet_Example.h"
#include "CvRDT/2PSet_Example.h"
#include "CvRDT/GMap_Example.h"
#include "CvRDT/GGraph_Example.h"

// CmRDT (Operation-based)
#include "CmRDT/LWWGraph_Example.h"
#include "CmRDT/LWWMap_Example.h"
#include "CmRDT/LWWRegister_Example.h"
#include "CmRDT/LWWSet_Example.h"

// collab
#include "collab/LWWViewMDE_Example.h"


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


    // CmRDT (Operation-based)

    CmRDT::LWWMap_example();
    CmRDT::LWWRegister_example();
    CmRDT::LWWSet_example();
    CmRDT::LWWGraph_example();


    // collab
    collab::LWWViewMDE_example();


    return 0;
}
