#include "collab/data/ViewMDE.h"
#include <gtest/gtest.h>

#include "collab/data/OperationObserver.h"


namespace collab {


class ViewMDEObserver : virtual public OperationObserver {
    public:
        void receiveOperation(const Operation& op) {
            // TODO
        }
};


TEST(ViewMDE, constructorTest) {
    ViewMDE v;
}

TEST(ViewMDE, addElementTest) {
    ViewMDE v;
    v.addElement("e1");
}

TEST(ViewMDE, removeElementTest) {
    ViewMDE v;
    v.addElement("e1");
    v.removeElement("e1");
}


} // End namespace


