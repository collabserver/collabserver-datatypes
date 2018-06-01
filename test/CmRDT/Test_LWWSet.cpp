#include "collab/CmRDT/LWWSet.h"
#include <gtest/gtest.h>


// Check the whole internal state of an element
#define _ASSERT_ELT_EQ(elt_it_, key_, is_removed_, stamp_, data_) \
    ASSERT_TRUE(elt_it_ != data_.crdt_end()); \
    EXPECT_EQ(elt_it_->first, key_); \
    EXPECT_EQ(elt_it_->second.isRemoved(), is_removed_); \
    EXPECT_EQ(elt_it_->second.timestamp(), stamp_)


namespace collab {
namespace CmRDT {


// -----------------------------------------------------------------------------
// empty()
// -----------------------------------------------------------------------------

TEST(LWWSet, emptyTest) {
    LWWSet<int, int> data0;
    ASSERT_TRUE(data0.empty());

    data0.add(1, 10);
    ASSERT_FALSE(data0.empty());
    data0.remove(1, 20);
    ASSERT_TRUE(data0.empty());
    data0.add(1, 15);
    data0.add(1, 17);
    data0.add(1, 16);
    ASSERT_TRUE(data0.empty());
    data0.add(2, 30);
    ASSERT_FALSE(data0.empty());
}


// -----------------------------------------------------------------------------
// size()
// -----------------------------------------------------------------------------
TEST(LWWSet, sizeTest) {
    LWWSet<int, int> data0;
    ASSERT_EQ(data0.size(), 0);

    // Add elements
    data0.add(1, 10);
    ASSERT_EQ(data0.size(), 1);
    data0.add(2, 11);
    ASSERT_EQ(data0.size(), 2);
    data0.add(3, 12);
    ASSERT_EQ(data0.size(), 3);
    data0.add(4, 13);
    ASSERT_EQ(data0.size(), 4);
    data0.add(5, 14);
    ASSERT_EQ(data0.size(), 5);
    data0.add(6, 15);
    ASSERT_EQ(data0.size(), 6);

    // Remove them all
    data0.remove(1, 20);
    ASSERT_EQ(data0.size(), 5);
    data0.remove(2, 21);
    ASSERT_EQ(data0.size(), 4);
    data0.remove(3, 22);
    ASSERT_EQ(data0.size(), 3);
    data0.remove(4, 23);
    ASSERT_EQ(data0.size(), 2);
    data0.remove(5, 24);
    ASSERT_EQ(data0.size(), 1);
    data0.remove(6, 25);
    ASSERT_EQ(data0.size(), 0);
}

TEST(LWWSet, sizeWithDuplicateAddTest) {
    LWWSet<int, int> data0;

    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.size(), 0);

    data0.add(1, 10);
    data0.add(1, 18);
    data0.add(1, 19);
    data0.add(1, 11);
    data0.add(1, 15);
    ASSERT_EQ(data0.size(), 1);
}

TEST(LWWSet, sizeWithDuplicateAddRemoveTest) {
    LWWSet<int, int> data0;

    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.size(), 0);

    data0.add(1, 11);
    data0.add(1, 18);
    ASSERT_EQ(data0.size(), 1);

    data0.remove(1, 10);
    ASSERT_EQ(data0.size(), 1);

    data0.remove(1, 20);
    ASSERT_EQ(data0.size(), 0);

    data0.remove(1, 32);
    data0.remove(1, 38);
    data0.remove(1, 39);
    data0.remove(1, 35);
    data0.remove(1, 31);
    ASSERT_EQ(data0.size(), 0);
}

TEST(LWWSet, sizeWithRemoveFirstTest) {
    LWWSet<int, int> data0;
    ASSERT_EQ(data0.size(), 0);

    data0.remove(1, 22);
    data0.remove(1, 28);
    data0.remove(1, 29);
    data0.remove(1, 25);
    data0.remove(1, 21);
    ASSERT_EQ(data0.size(), 0); // 0, not -5 or shit

    data0.add(1, 10);
    ASSERT_EQ(data0.size(), 0);

    data0.add(1, 30);
    ASSERT_EQ(data0.size(), 1);

    data0.remove(2, 10);
    data0.remove(2, 45);
    data0.remove(2, 40);
    ASSERT_EQ(data0.size(), 1);
}

TEST(LWWSet, sizeWithOlderRemoveAfterAddTest) {
    LWWSet<int, int> data0;
    ASSERT_EQ(data0.size(), 0);

    data0.remove(1, 10);
    ASSERT_EQ(data0.size(), 0);
    data0.add(1, 20);
    ASSERT_EQ(data0.size(), 1);
    data0.remove(1, 11);
    data0.remove(1, 12);
    data0.remove(1, 18);
    ASSERT_EQ(data0.size(), 1); // Not removed (See timestamps)
}


// -----------------------------------------------------------------------------
// count()
// -----------------------------------------------------------------------------
TEST(LWWSet, countTest) {
    LWWSet<int, int> data0;

    ASSERT_EQ(data0.count(32), 0);
    ASSERT_EQ(data0.count(10), 0);
    ASSERT_EQ(data0.count(42), 0);

    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);

    ASSERT_EQ(data0.count(1), 1);
    ASSERT_EQ(data0.count(2), 1);
    ASSERT_EQ(data0.count(3), 1);
    ASSERT_EQ(data0.count(32), 0);
    ASSERT_EQ(data0.count(10), 0);
    ASSERT_EQ(data0.count(42), 0);
}

TEST(LWWset, countAfterRemoveTest) {
    LWWSet<int, int> data0;

    ASSERT_EQ(data0.count(42), 0);
    data0.add(42, 10);
    data0.remove(42, 20);
    ASSERT_EQ(data0.count(42), 0);
}


// -----------------------------------------------------------------------------
// max_size()
// -----------------------------------------------------------------------------

TEST(LWWSet, max_sizeTest) {
    // Well, this is not really a test, my goal here is just to call max_size
    // (So that I'm sure it's compiling). But max_size itself is just a 
    // 'foward' of unordered_map::max_size. See code.
    LWWSet<int, int> data0;
    LWWSet<int,int>::size_type t = data0.max_size();
    ASSERT_TRUE(t > 0);
}


// -----------------------------------------------------------------------------
// query()
// -----------------------------------------------------------------------------

TEST(LWWSet, queryTest) {
    LWWSet<std::string, int> data0;

    // Query before exists
    auto coco = data0.query("e1");
    EXPECT_TRUE(coco == data0.crdt_end());

    // Add element and query
    data0.add("e1", 10);
    coco = data0.query("e1");
    _ASSERT_ELT_EQ(coco, "e1", false, 10, data0);

    // Remove this element and query
    data0.remove("e1", 20);
    coco = data0.query("e1");
    _ASSERT_ELT_EQ(coco, "e1", true, 20, data0);

    // Query invalid data
    coco = data0.query("xxx");
    EXPECT_TRUE(coco == data0.crdt_end());
}


// -----------------------------------------------------------------------------
// find()
// -----------------------------------------------------------------------------

TEST(LWWSet, findTest) {
    LWWSet<std::string, int> data0;

    // Add all the crap there!
    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);

    // Find them all!
    auto e1 = data0.find("e1");
    auto e2 = data0.find("e2");
    auto e3 = data0.find("e3");
    EXPECT_EQ(*e1, "e1");
    EXPECT_EQ(*e2, "e2");
    EXPECT_EQ(*e3, "e3");
}

TEST(LWWSet, findRemovedElementTest) {
    LWWSet<std::string, int> data0;

    data0.add("e1", 10);
    data0.remove("e1", 20);
    auto e1 = data0.find("e1");
    EXPECT_EQ(e1, data0.end());
}


// -----------------------------------------------------------------------------
// add()
// -----------------------------------------------------------------------------

TEST(LWWSet, addTest) {
    LWWSet<int, int> data0;

    // Add normal
    data0.add(0, 10);
    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);
    for(int k = 0; k < 4; ++k) {
        auto coco = data0.query(k);
        _ASSERT_ELT_EQ(coco, k, false, 10, data0);
    }

    // More add later
    data0.add(0, 20);
    data0.add(1, 20);
    data0.add(2, 20);
    data0.add(3, 20);
    for(int k = 0; k < 4; ++k) {
        auto coco = data0.query(k);
        _ASSERT_ELT_EQ(coco, k, false, 20, data0);
    }
}

TEST(LWWSet, addDuplicateCallsTest) {
    LWWSet<int, int> data0;

    // Test duplicate add, keep max timestamps 
    data0.add(42, 15);
    data0.add(42, 14);
    data0.add(42, 18);
    data0.add(42, 19);
    data0.add(42, 17);
    data0.add(42, 10);
    auto carrot = data0.query(42);
    _ASSERT_ELT_EQ(carrot, 42, false, 19, data0);

    // Test duplicate add, keep max timestamps 
    data0.add(64, 28);
    data0.add(64, 29);
    data0.add(64, 21);
    data0.add(64, 22);
    data0.add(64, 27);
    data0.add(64, 25);
    carrot = data0.query(64);
    _ASSERT_ELT_EQ(carrot, 64, false, 29, data0);
}


// -----------------------------------------------------------------------------
// remove()
// -----------------------------------------------------------------------------

TEST(LWWSet, removeTest) {
    LWWSet<int, int> data0;

    // Some remove after add. Check is marked as removed.
    data0.add(0, 10);
    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);
    data0.remove(0, 20);
    data0.remove(1, 20);
    data0.remove(2, 20);
    data0.remove(3, 20);
    for(int k = 0; k < 4; ++k) {
        auto res = data0.query(k);
        _ASSERT_ELT_EQ(res, k, true, 20, data0);
    }
}

TEST(LWWSet, removeDuplicateCallsTest) {
    LWWSet<int, int> data0;

    // Duplicate remove, set higher timestamps
    data0.add(42, 10);
    data0.remove(42, 23);
    data0.remove(42, 22);
    data0.remove(42, 27);
    data0.remove(42, 22);
    data0.remove(42, 29);
    data0.remove(42, 21);
    auto res = data0.query(42);
    _ASSERT_ELT_EQ(res, 42, true, 29, data0);
}

TEST(LWWSet, removeCalledBeforeAddCallTest) {
    LWWSet<int, int> data0;

    // Remove before even added works
    data0.remove(42, 10);
    auto res = data0.query(42);
    _ASSERT_ELT_EQ(res, 42, true, 10, data0);

    // Re-removed later, change timestamps anyway
    data0.remove(42, 20);
    res = data0.query(42);
    _ASSERT_ELT_EQ(res, 42, true, 20, data0);
}


// -----------------------------------------------------------------------------
// add() + remove()
// -----------------------------------------------------------------------------

TEST(LWWSet, addRemoveTest) {
    LWWSet<std::string, int> data0;

    // Add element
    data0.add("v1", 10);
    auto res = data0.query("v1");
    _ASSERT_ELT_EQ(res, "v1", false, 10, data0);

    // Remove this element
    data0.remove("v1", 20);
    res = data0.query("v1");
    _ASSERT_ELT_EQ(res, "v1", true, 20, data0);

    // Re-add
    data0.add("v1", 30);
    res = data0.query("v1");
    _ASSERT_ELT_EQ(res, "v1", false, 30, data0);

    // Re-remove
    data0.remove("v1", 40);
    res = data0.query("v1");
    _ASSERT_ELT_EQ(res, "v1", true, 40, data0);
}

TEST(LWWSet, addRemoveWithRemoveCalledFirstTest) {
    LWWSet<std::string, int> data0;

    // Remove elt before even added. (Is technically removed)
    data0.remove("v1", 1000);
    auto res = data0.query("v1");
    _ASSERT_ELT_EQ(res, "v1", true, 1000, data0);

    // Add this element, but remove was done later (Still removed)
    data0.add("v1", 10);
    res = data0.query("v1");
    _ASSERT_ELT_EQ(res, "v1", true, 1000, data0);

    // Re-add this element after the remove
    data0.add("v1", 1001);
    res = data0.query("v1");
    _ASSERT_ELT_EQ(res, "v1", false, 1001, data0);

    // Remove this element before the last add: do nothing (Still added)
    data0.remove("v1", 20);
    res = data0.query("v1");
    _ASSERT_ELT_EQ(res, "v1", false, 1001, data0);
}

TEST(LWWSet, addRemoveUseCaseTest) {
    LWWSet<std::string, int> data0;
    LWWSet<std::string, int> data1;

    // User1 flow (Normal order)
    data0.add("v1", 1);
    data0.add("v2", 3);
    data0.remove("v1", 4);

    // User1 flow (receive remove before add)
    data1.remove("v1", 4);
    data1.add("v2", 3);
    data1.add("v1", 1);

    // They are same (User point of view and internally as well)
    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);
    EXPECT_TRUE(data0.crdt_equal(data1));
}


// -----------------------------------------------------------------------------
// reserve()
// -----------------------------------------------------------------------------

TEST(LWWSet, reserveTest) {
    LWWSet<int, int> data0;

    // Not sure how to test it at this exact terrible moment.
    // For now, just do a call to be sure it compiles.
    data0.reserve(10);
}


// -----------------------------------------------------------------------------
// crdt_size()
// -----------------------------------------------------------------------------

TEST(LWWSet, crdt_sizeTest) {
    LWWSet<int, int> data0;

    // Add element and test
    ASSERT_EQ(data0.crdt_size(), 0);
    data0.add(1, 10);
    ASSERT_EQ(data0.crdt_size(), 1);
    data0.add(2, 20);
    ASSERT_EQ(data0.crdt_size(), 2);
    data0.add(3, 30);
    ASSERT_EQ(data0.crdt_size(), 3);

    // Remove element, size won't change.
    data0.remove(1, 100);
    ASSERT_EQ(data0.crdt_size(), 3);
    data0.remove(2, 200);
    ASSERT_EQ(data0.crdt_size(), 3);
    data0.remove(3, 300);
    ASSERT_EQ(data0.crdt_size(), 3);
}

TEST(LWWSet, crdt_equal) {
    LWWSet<std::string, int> data0;
    LWWSet<std::string, int> data1;

    ASSERT_TRUE(data0.crdt_equal(data1));

    // Add some element in data1, data1 != data2 then
    data0.add("e1", 10);
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));
    data0.add("e2", 20);
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast to data1, they are same again! (Yeah! So beautiful!)
    data1.add("e1", 10);
    data1.add("e2", 20);
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));

    // Just little test, but yeah, data0 is equal to himself (and so data1)
    ASSERT_TRUE(data0.crdt_equal(data0));
    ASSERT_TRUE(data1.crdt_equal(data1));
}


// -----------------------------------------------------------------------------
// Iterator Tests (Normal iterator)
// -----------------------------------------------------------------------------

TEST(LWWSet, iteratorAddRemoveTest) {
    LWWSet<int, int> data0;

    // Add some elements and test iteration
    data0.add(0, 10);
    data0.add(1, 11);
    data0.add(2, 12);
    data0.add(3, 13);
    int k = 0;
    for(auto it = data0.begin(); it != data0.end(); ++it) {
        // Dev note: I'm not sure order is predictable.
        // I use number of iteration instead.
        ++k;
    }
    EXPECT_EQ(k, 4);

    // Remove elements, then iterator should not use them
    data0.remove(0, 20);
    data0.remove(1, 21);
    k = 0;
    for(auto& elt : data0) {
        ++k;
    }
    EXPECT_EQ(k, 2);

    // Add again some more and test
    data0.add(4, 30);
    data0.add(5, 31);
    data0.add(6, 32);
    data0.add(7, 33);
    k = 0;
    for(auto& elt : data0) {
        ++k;
    }
    EXPECT_EQ(k, 6);
}

TEST(LWWSet, iteratorEmptySetTest) {
    LWWSet<int, int> data0;

    // Iterate empty set should be ok (No elt)
    for(auto& elt : data0) {
        EXPECT_TRUE(false) << "Iterator should be empty but found " << elt;
    }

    // Add / remove some elements and iterate
    data0.add(0, 10);
    data0.add(1, 11);
    data0.add(2, 12);
    data0.add(3, 13);
    data0.add(4, 14);
    data0.remove(0, 20);
    data0.remove(1, 21);
    data0.remove(2, 22);
    data0.remove(3, 23);
    data0.remove(4, 24);
    for(auto& elt : data0) {
        // data0 should be empty from iterator point of view
        ASSERT_TRUE(false) << "Iterator should be empty but found " << elt;
    }

    // Add more and remove
    data0.add(5, 30);
    data0.remove(5, 31);
    for(auto& elt : data0) {
        // Should be Still empty
        ASSERT_TRUE(false) << "Iterator should be empty but found " << elt;
    }

    // Add / remove again
    data0.add(6, 40);
    data0.add(7, 41);
    data0.add(8, 42);
    data0.add(9, 43);
    data0.remove(6, 44);
    data0.remove(7, 45);
    data0.remove(8, 46);
    data0.remove(9, 47);
    for(auto& elt : data0) {
        // Should be Still empty
        ASSERT_TRUE(false) << "Iterator should be empty but found " << elt;
    }
}

TEST(LWWSet, iteratorReferenceTest) {
    LWWSet<int, int> data0;

    // Add all
    data0.add(1, 1);
    auto it = data0.begin();
    EXPECT_EQ(*it, 1);
}


// -----------------------------------------------------------------------------
// Iterator Tests (crdt iterator)
// -----------------------------------------------------------------------------

TEST(LWWSet, crdtIteratorAddRemoveTest) {
    LWWSet<int, int> data0;

    // Add some elements and test iteration
    data0.add(0, 10);
    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);
    int k = 0;
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        // Dev note: I'm not sure order is predictable.
        // I use number of iterations instead.
        ++k;
        EXPECT_FALSE(it->second.isRemoved());
        EXPECT_EQ(it->second.timestamp(), 10);
    }
    EXPECT_EQ(k, 4);

    // Remove elements, should not impact crdt iterator
    data0.remove(0, 20);
    data0.remove(1, 21);
    k = 0;
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ++k;
    }
    EXPECT_EQ(k, 4);

    // Add / remove some more
    data0.add(4, 30);
    data0.add(5, 31);
    data0.add(6, 32);
    data0.add(7, 33);
    data0.remove(4, 34);
    data0.remove(5, 35);
    k = 0;
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ++k;
    }
    EXPECT_EQ(k, 8);
}

TEST(LWWSet, crdtIteratorEmptyTest) {
    LWWSet<int, int> data0;

    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ASSERT_TRUE(false) << "crdt Iterator should be empty";;
    }
}

TEST(LWWSet, crdtIteratorRemovedTest) {
    LWWSet<int, int> data0;

    // Fill set with removed elt (Yes, we don't even need add before).
    data0.remove(1, 10);
    data0.remove(2, 10);
    data0.remove(3, 10);
    data0.remove(4, 10);
    data0.remove(5, 10);
    int k = 0;
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ++k;
        EXPECT_TRUE(it->second.isRemoved());
        EXPECT_EQ(it->second.timestamp(), 10);
    }
    ASSERT_EQ(k, 5);

    // Add all
    data0.add(1, 20);
    data0.add(2, 20);
    data0.add(3, 20);
    data0.add(4, 20);
    data0.add(5, 20);
    k = 0;
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ++k;
        EXPECT_FALSE(it->second.isRemoved());
        EXPECT_EQ(it->second.timestamp(), 20);
    }
    ASSERT_EQ(k, 5);

    // Add re-remove them all
    data0.remove(1, 30);
    data0.remove(2, 30);
    data0.remove(3, 30);
    data0.remove(4, 30);
    data0.remove(5, 30);
    k = 0;
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ++k;
        EXPECT_TRUE(it->second.isRemoved());
        EXPECT_EQ(it->second.timestamp(), 30);
    }
    ASSERT_EQ(k, 5);
}

TEST(LWWSet, crdtIteratorReferenceTest) {
    LWWSet<int, int> data0;

    // Simple test add then remove
    data0.add(1, 10);
    auto it = data0.crdt_begin();
    EXPECT_FALSE(it->second.isRemoved());
    EXPECT_EQ(it->second.timestamp(), 10);
    it = data0.crdt_begin();
    data0.remove(1, 20);
    EXPECT_TRUE(it->second.isRemoved());
    EXPECT_EQ(it->second.timestamp(), 20);

    // Add all
    data0.add(1, 30);
    data0.add(2, 30);
    data0.add(3, 30);
    data0.add(4, 30);
    data0.add(5, 30);
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        EXPECT_FALSE(it->second.isRemoved());
        EXPECT_EQ(it->second.timestamp(), 30);
    }

    // Remove all
    data0.remove(1, 40);
    data0.remove(2, 40);
    data0.remove(3, 40);
    data0.remove(4, 40);
    data0.remove(5, 40);
    for(auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        EXPECT_TRUE(it->second.isRemoved());
        EXPECT_EQ(it->second.timestamp(), 40);
    }
}


// -----------------------------------------------------------------------------
// Operator Tests
// -----------------------------------------------------------------------------

TEST(LWWSet, operatorEQTest) {
    LWWSet<std::string, int> data0;
    LWWSet<std::string, int> data1;

    // Simple add, both are same (Timestamp doesn't count)
    data0.add("v1", 1);
    data1.add("v1", 2);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);

    // Removed elt not used in equality
    data0.remove("v2", 3);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);

    // Remove in all
    data1.remove("v2", 3);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);

    // Some more add (Still not equal)
    data0.add("v3", 4);
    data0.add("v4", 5);
    data0.add("v5", 6);
    data1.add("v3", 7);
    data1.add("v4", 8);
    ASSERT_FALSE(data0 == data1);
    ASSERT_TRUE(data0 != data1);

    // Now are equal
    data1.add("v5", 9);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);
}


}} // End namespaces


