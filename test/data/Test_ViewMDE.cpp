#include "collab/data/ViewMDE.h"
#include <gtest/gtest.h>



namespace collab {


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

TEST(ViewMDE, addAttributeTest) {
    ViewMDE v;
    v.addElement("e1");
    v.addElement("e2");

    v.addAttribute("e1", "attr1", "value1");
    v.addAttribute("e1", "attr2", "value2");

    v.addAttribute("e2", "attr1", "value1");
    v.addAttribute("e2", "attr2", "value2");
}

TEST(ViewMDE, removeAttributeTest) {
    ViewMDE v;
    v.addElement("e1");
    v.addAttribute("e1", "attr1", "value1");
    v.removeAttribute("e1", "attr1");
}

TEST(ViewMDE, updateAttributeTest) {
    ViewMDE v;
    v.addElement("e1");
    v.addAttribute("e1", "attr1", "value1");
    v.updateAttribute("e1", "attr1", "newValue1");
}

TEST(ViewMDE, addLinkTest) {
    ViewMDE v;
    v.addElement("e1");
    v.addElement("e2");
    v.addLink("e1", "e2");
}

TEST(ViewMDE, removeLinkTest) {
    ViewMDE v;
    v.addElement("e1");
    v.addElement("e2");
    v.addLink("e1", "e2");
    v.removeLink("e1", "e2");
}


} // End namespace


