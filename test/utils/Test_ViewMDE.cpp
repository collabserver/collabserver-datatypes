#include "collabdata/utils/ViewMDE.h"

#include <gtest/gtest.h>

#include "collabdata/utils/OperationObserver.h"


namespace collab {


class ViewMDEObserver : virtual public OperationObserver {
    public:
        void receiveOperation(const Operation& op) {
            // TODO
        }
};


TEST(ViewMDE, constructorTest) {
    ViewMDE v;
    // TODO
}

TEST(ViewMDE, addElementTest) {
    ViewMDE v;
    v.addElement("e1");
    // TODO
}

TEST(ViewMDE, removeElementTest) {
    ViewMDE v;
    v.addElement("e1");
    v.removeElement("e1");
    // TODO
}


} // End namespace


