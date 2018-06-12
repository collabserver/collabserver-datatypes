#include "collabdata/CmRDT/LWWMap.h"

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

TEST(LWWMap, emptyTest) {
    LWWMap<int, int, int> data0;
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
// crdt_empty()
// -----------------------------------------------------------------------------

TEST(LWWMap, crdt_emptyTest) {
    LWWMap<int, int, int> data0;
    ASSERT_TRUE(data0.crdt_empty());

    data0.add(1, 10);
    ASSERT_FALSE(data0.crdt_empty());
    data0.remove(1, 20);
    ASSERT_FALSE(data0.crdt_empty());
    data0.add(1, 15);
    data0.add(1, 17);
    data0.add(1, 16);
    ASSERT_FALSE(data0.crdt_empty());
    data0.add(2, 30);
    ASSERT_FALSE(data0.crdt_empty());
}


// -----------------------------------------------------------------------------
// size()
// -----------------------------------------------------------------------------

TEST(LWWMap, sizeTest) {
    LWWMap<int, int, int> data0;
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

TEST(LWWMap, sizeWithDuplicateAddTest) {
    LWWMap<int, int, int> data0;

    ASSERT_EQ(data0.size(), 0);
    ASSERT_EQ(data0.size(), 0);

    data0.add(1, 10);
    data0.add(1, 18);
    data0.add(1, 19);
    data0.add(1, 11);
    data0.add(1, 15);
    ASSERT_EQ(data0.size(), 1);
}

TEST(LWWMap, sizeWithDuplicateAddRemoveTest) {
    LWWMap<int, int, int> data0;

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

TEST(LWWMap, sizeWithRemoveFirstTest) {
    LWWMap<int, int, int> data0;
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

TEST(LWWMap, sizeWithOlderRemoveAfterAddTest) {
    LWWMap<int, int, int> data0;
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
// max_size()
// -----------------------------------------------------------------------------

TEST(LWWMap, max_sizeTest) {
    // Well, this is not really a test, my goal here is just to call max_size
    // (So that I'm sure it's compiling). But max_size itself is just a 
    // 'foward' of unordered_map::max_size. See code.
    LWWMap<int, int, int> data0;
    LWWMap<int, int, int>::size_type t = data0.max_size();
    ASSERT_TRUE(t > 0);
}


// -----------------------------------------------------------------------------
// count()
// -----------------------------------------------------------------------------
TEST(LWWMap, countTest) {
    LWWMap<std::string, int, int> data0;

    ASSERT_EQ(data0.count("x1"), 0);
    ASSERT_EQ(data0.count("x2"), 0);
    ASSERT_EQ(data0.count("x3"), 0);

    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);

    ASSERT_EQ(data0.count("e1"), 1);
    ASSERT_EQ(data0.count("e2"), 1);
    ASSERT_EQ(data0.count("e3"), 1);
    ASSERT_EQ(data0.count("x1"), 0);
    ASSERT_EQ(data0.count("x2"), 0);
    ASSERT_EQ(data0.count("x3"), 0);
}

TEST(LWWMap, countAfterRemoveTest) {
    LWWMap<int, int, int> data0;

    ASSERT_EQ(data0.count(42), 0);
    data0.add(42, 10);
    data0.remove(42, 20);
    ASSERT_EQ(data0.count(42), 0);
}


// -----------------------------------------------------------------------------
// crdt_count()
// -----------------------------------------------------------------------------
TEST(LWWMap, crdt_countTest) {
    LWWMap<int, int, int> data0;

    ASSERT_EQ(data0.crdt_count(1), 0);
    ASSERT_EQ(data0.crdt_count(2), 0);
    ASSERT_EQ(data0.crdt_count(3), 0);

    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);
    data0.remove(1, 20);
    data0.remove(2, 20);
    data0.remove(3, 20);

    ASSERT_EQ(data0.crdt_count(1), 1);
    ASSERT_EQ(data0.crdt_count(2), 1);
    ASSERT_EQ(data0.crdt_count(3), 1);
    ASSERT_EQ(data0.crdt_count(32), 0);
    ASSERT_EQ(data0.crdt_count(10), 0);
    ASSERT_EQ(data0.crdt_count(42), 0);
}

TEST(LWWMap, crdt_countAfterRemoveTest) {
    LWWMap<int, int, int> data0;

    ASSERT_EQ(data0.crdt_count(42), 0);
    data0.add(42, 10);
    data0.remove(42, 20);
    ASSERT_EQ(data0.crdt_count(42), 1);
}


// -----------------------------------------------------------------------------
// crdt_find()
// -----------------------------------------------------------------------------

TEST(LWWMap, crdt_findTest) {
    LWWMap<std::string, int, int> data0;

    // Query before exists
    auto coco = data0.crdt_find("e1");
    EXPECT_TRUE(coco == data0.crdt_end());

    // Add element and query
    data0.add("e1", 10);
    coco = data0.crdt_find("e1");
    _ASSERT_ELT_EQ(coco, "e1", false, 10, data0);

    // Remove this element and query
    data0.remove("e1", 20);
    coco = data0.crdt_find("e1");
    _ASSERT_ELT_EQ(coco, "e1", true, 20, data0);

    // Query invalid data
    coco = data0.crdt_find("xxx");
    EXPECT_TRUE(coco == data0.crdt_end());
}

TEST(LWWMap, queryAndChangeValueTests) {
    LWWMap<std::string, int, int> data0;

    // Setup data
    data0.add("e1", 1);
    data0.add("e2", 2);
    data0.remove("e1", 3);
    data0.remove("e2", 3);

    // Change values of container
    auto it_e1 = data0.crdt_find("e1");
    it_e1->second.value() = 42;
    auto it_e2 = data0.crdt_find("e2");
    it_e2->second.value() = 1024;

    // Check if container value are well changed
    auto e1 = data0.crdt_find("e1");
    auto e2 = data0.crdt_find("e2");
    EXPECT_EQ(e1->second.value(), 42);
    EXPECT_EQ(e2->second.value(), 1024);
}


// -----------------------------------------------------------------------------
// find()
// -----------------------------------------------------------------------------

TEST(LWWMap, findTest) {
    LWWMap<std::string, int, int> data0;

    // Add all the crap there!
    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);

    // Find them all!
    auto e1 = data0.find("e1");
    auto e2 = data0.find("e2");
    auto e3 = data0.find("e3");

    EXPECT_EQ(e1->first, "e1");
    EXPECT_EQ((*e1).first, "e1");
    EXPECT_EQ((*e2).first, "e2");
    EXPECT_EQ((*e3).first, "e3");
}

TEST(LWWMap, findRemovedElementTest) {
    LWWMap<std::string, int, int> data0;

    data0.add("e1", 10);
    data0.remove("e1", 20);
    auto e1 = data0.find("e1");
    EXPECT_EQ(e1, data0.end());
}

TEST(LWWMap, findAndChangeValueTests) {
    LWWMap<std::string, int, int> data0;

    // Setup data
    data0.add("e1", 1); // v1 == default set value
    data0.add("e2", 2); // v2 == default set value

    // Change values of container
    auto it_e1 = data0.find("e1");
    it_e1->second = 42;
    auto it_e2 = data0.find("e2");
    it_e2->second = 1024;

    // Check if container value are well changed
    auto e1 = data0.find("e1");
    auto e2 = data0.find("e2");
    EXPECT_EQ(e1->second, 42);
    EXPECT_EQ(e2->second, 1024);
}


// -----------------------------------------------------------------------------
// add()
// -----------------------------------------------------------------------------

TEST(LWWMap, addTest) {
    LWWMap<int, int, int> data0;

    // Add normal
    data0.add(0, 10);
    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);
    for(int k = 0; k < 4; ++k) {
        auto coco = data0.crdt_find(k);
        _ASSERT_ELT_EQ(coco, k, false, 10, data0);
    }

    // More add later
    data0.add(0, 20);
    data0.add(1, 20);
    data0.add(2, 20);
    data0.add(3, 20);
    for(int k = 0; k < 4; ++k) {
        auto coco = data0.crdt_find(k);
        _ASSERT_ELT_EQ(coco, k, false, 20, data0);
    }
}

TEST(LWWMap, addDuplicateCallsTest) {
    LWWMap<int, int, int> data0;

    // Test duplicate add, keep max timestamps 
    data0.add(42, 15);
    data0.add(42, 14);
    data0.add(42, 18);
    data0.add(42, 19);
    data0.add(42, 17);
    data0.add(42, 10);
    auto carrot = data0.crdt_find(42);
    _ASSERT_ELT_EQ(carrot, 42, false, 19, data0);

    // Test duplicate add, keep max timestamps 
    data0.add(64, 28);
    data0.add(64, 29);
    data0.add(64, 21);
    data0.add(64, 22);
    data0.add(64, 27);
    data0.add(64, 25);
    carrot = data0.crdt_find(64);
    _ASSERT_ELT_EQ(carrot, 64, false, 29, data0);
}

TEST(LWWMap, addReturnTypeTest) {
    LWWMap<std::string, int, int> data0;

    // First add the element.
    ASSERT_TRUE(data0.add("coco", 20));

    // Now, only update timestamp, but was already added
    ASSERT_FALSE(data0.add("coco", 10));
    ASSERT_FALSE(data0.add("coco", 15));
    ASSERT_FALSE(data0.add("coco", 32));
    ASSERT_FALSE(data0.add("coco", 64));

    // Another test for fun
    ASSERT_TRUE(data0.add("carrot", 1024));
    ASSERT_FALSE(data0.add("carrot", 1023));
    ASSERT_FALSE(data0.add("carrot", 2048));
}

TEST(LWWMap, addReturnTypeWithRemoveCalled) {
    LWWMap<std::string, int, int> data0;

    ASSERT_TRUE(data0.add("coco", 10));

    data0.remove("coco", 42);
    ASSERT_FALSE(data0.add("coco", 20));
    ASSERT_FALSE(data0.add("coco", 30));
    ASSERT_FALSE(data0.add("coco", 40));
    ASSERT_TRUE(data0.add("coco", 50));

    data0.remove("coco", 10);
    ASSERT_FALSE(data0.add("coco", 60));

    data0.remove("coco", 512);
    ASSERT_FALSE(data0.add("coco", 70));
    ASSERT_FALSE(data0.add("coco", 511));
    ASSERT_TRUE(data0.add("coco", 513));
}


// -----------------------------------------------------------------------------
// remove()
// -----------------------------------------------------------------------------

TEST(LWWMap, removeTest) {
    LWWMap<int, int, int> data0;

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
        auto res = data0.crdt_find(k);
        _ASSERT_ELT_EQ(res, k, true, 20, data0);
    }
}

TEST(LWWMap, removeDuplicateCallsTest) {
    LWWMap<int, int, int> data0;

    // Duplicate remove, set higher timestamps
    data0.add(42, 10);
    data0.remove(42, 23);
    data0.remove(42, 22);
    data0.remove(42, 27);
    data0.remove(42, 22);
    data0.remove(42, 29);
    data0.remove(42, 21);
    auto res = data0.crdt_find(42);
    _ASSERT_ELT_EQ(res, 42, true, 29, data0);
}

TEST(LWWMap, removeCalledBeforeAddCallTest) {
    LWWMap<int, int, int> data0;

    // Remove before even added works
    data0.remove(42, 10);
    auto res = data0.crdt_find(42);
    _ASSERT_ELT_EQ(res, 42, true, 10, data0);

    // Re-removed later, change timestamps anyway
    data0.remove(42, 20);
    res = data0.crdt_find(42);
    _ASSERT_ELT_EQ(res, 42, true, 20, data0);
}

TEST(LWWMap, removeCalledFirstReturnTypeTest) {
    LWWMap<std::string, int, int> data0;

    ASSERT_FALSE(data0.remove("coco", 20));
    ASSERT_FALSE(data0.remove("coco", 10));
    ASSERT_FALSE(data0.remove("coco", 30));

    data0.add("coco", 15);
    ASSERT_FALSE(data0.remove("coco", 40));

    data0.add("coco", 80);
    ASSERT_TRUE(data0.remove("coco", 90));
}

TEST(LWWMap, removeReturnTypeTest) {
    LWWMap<std::string, int, int> data0;

    // Normal add / remove test
    data0.add("coco", 20);
    ASSERT_FALSE(data0.remove("coco", 10));
    ASSERT_TRUE(data0.remove("coco", 30));

    // Duplicate remove
    ASSERT_FALSE(data0.remove("coco", 40));
    ASSERT_FALSE(data0.remove("coco", 50));
    ASSERT_FALSE(data0.remove("coco", 60));
}


// -----------------------------------------------------------------------------
// add() + remove()
// -----------------------------------------------------------------------------

TEST(LWWMap, addRemoveTest) {
    LWWMap<std::string, int, int> data0;

    // Add element
    data0.add("v1", 10);
    auto res = data0.crdt_find("v1");
    _ASSERT_ELT_EQ(res, "v1", false, 10, data0);

    // Remove this element
    data0.remove("v1", 20);
    res = data0.crdt_find("v1");
    _ASSERT_ELT_EQ(res, "v1", true, 20, data0);

    // Re-add
    data0.add("v1", 30);
    res = data0.crdt_find("v1");
    _ASSERT_ELT_EQ(res, "v1", false, 30, data0);

    // Re-remove
    data0.remove("v1", 40);
    res = data0.crdt_find("v1");
    _ASSERT_ELT_EQ(res, "v1", true, 40, data0);
}

TEST(LWWMap, addRemoveWithRemoveCalledFirstTest) {
    LWWMap<std::string, int, int> data0;

    // Remove elt before even added. (Is technically removed)
    data0.remove("v1", 1000);
    auto res = data0.crdt_find("v1");
    _ASSERT_ELT_EQ(res, "v1", true, 1000, data0);

    // Add this element, but remove was done later (Still removed)
    data0.add("v1", 10);
    res = data0.crdt_find("v1");
    _ASSERT_ELT_EQ(res, "v1", true, 1000, data0);

    // Re-add this element after the remove
    data0.add("v1", 1001);
    res = data0.crdt_find("v1");
    _ASSERT_ELT_EQ(res, "v1", false, 1001, data0);

    // Remove this element before the last add: do nothing (Still added)
    data0.remove("v1", 20);
    res = data0.crdt_find("v1");
    _ASSERT_ELT_EQ(res, "v1", false, 1001, data0);
}

TEST(LWWMap, addRemoveUseCaseTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // User1 flow (Normal order)
    data0.add("v1", 1);
    data0.add("v2", 3);
    data0.remove("v1", 4);

    // User1 flow (Remove before add)
    data1.remove("v1", 4);
    data1.add("v2", 3);
    data1.add("v1", 1);

    EXPECT_TRUE(data0 == data1);
    EXPECT_FALSE(data0 != data1);
}


// -----------------------------------------------------------------------------
// reserve()
// -----------------------------------------------------------------------------

TEST(LWWMap, reserveTest) {
    LWWMap<int, int, int> data0;

    // Not sure how to test it at this exact terrible moment.
    // For now, just do a call to be sure it compiles.
    data0.reserve(10);
}


// -----------------------------------------------------------------------------
// crdt_size()
// -----------------------------------------------------------------------------

TEST(LWWMap, crdt_sizeTest) {
    LWWMap<int, int, int> data0;

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


// -----------------------------------------------------------------------------
// crdt_equal()
// -----------------------------------------------------------------------------

TEST(LWWMap, crdt_equalWithOnlyAddTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

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

TEST(LWWMap, crdt_equalWithAddRemoveTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // Operations on data0
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);
    data0.remove("e2", 20);
    data0.remove("e3", 20);
    data0.add("e4", 20);

    // Operations on data1
    data1.add("e1", 11);
    data1.add("e6", 11);
    data1.add("e7", 11);
    data1.remove("e6", 21);
    data1.add("e8", 21);

    // atm, not equals
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast data0 to data1
    data1.add("e1", 10);
    data1.add("e2", 10);
    data1.add("e3", 10);
    data1.remove("e2", 20);
    data1.remove("e3", 20);
    data1.add("e4", 20);

    // Broadcast data1 to data0
    data0.add("e1", 11);
    data0.add("e6", 11);
    data0.add("e7", 11);
    data0.remove("e6", 21);
    data0.add("e8", 21);

    // Now they are same! Incredible
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));
}

TEST(LWWMap, crdt_equalSameValueButDifferentTimestampTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    data0.add("e1", 10);
    data1.add("e2", 20);

    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast
    data1.add("e1", 10);
    data0.add("e2", 20);

    // Now are same
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));
}

TEST(LWWMap, crdt_equalWithUsersameButInternalNotSameTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // data0
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);
    data0.remove("e3", 20);

    // data1
    data1.add("e1", 30);
    data1.add("e2", 30);

    // They are different
    // (Same for user point of view, but not same internally)
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast data0 to data1
    data1.add("e1", 10);
    data1.add("e2", 10);
    data1.add("e3", 10);
    data1.remove("e3", 20);

    // Broadcast data1 to data0
    data0.add("e1", 30);
    data0.add("e2", 30);

    // Now are same
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));
}

TEST(LWWMap, crdt_equalEmptyVsAddTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));

    // Add only in data0
    data0.add("e1", 10);
    data0.add("e2", 20);
    data0.add("e3", 30);
    ASSERT_FALSE(data0.crdt_equal(data1));
    ASSERT_FALSE(data1.crdt_equal(data0));

    // Broadcast in data1
    data1.add("e1", 10);
    data1.add("e2", 20);
    data1.add("e3", 30);
    ASSERT_TRUE(data0.crdt_equal(data1));
    ASSERT_TRUE(data1.crdt_equal(data0));
}


// -----------------------------------------------------------------------------
// Iterator Tests (Normal iterator)
// -----------------------------------------------------------------------------

TEST(LWWMap, iteratorTest) {
    LWWMap<int, int, int> data0;

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

TEST(LWWMap, iteratorEmptyMapTest) {
    LWWMap<int, int, int> data0;

    // Iterate empty set should be ok (No elt)
    for(LWWMap<int,int,int>::iterator it = data0.begin(); it != data0.end(); ++it) {
        EXPECT_TRUE(false) << "Iterator should be empty but found ";
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
    for(LWWMap<int,int,int>::iterator it = data0.begin(); it != data0.end(); ++it) {
        // data0 should be empty from iterator point of view
        ASSERT_TRUE(false) << "Iterator should be empty but elt found ";
    }

    // Add more and remove
    data0.add(5, 30);
    data0.remove(5, 31);
    for(LWWMap<int,int,int>::iterator it = data0.begin(); it != data0.end(); ++it) {
        // Should be Still empty
        ASSERT_TRUE(false) << "Iterator should be empty but elt found ";
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
    for(LWWMap<int,int,int>::iterator it = data0.begin(); it != data0.end(); ++it) {
        // Should be Still empty
        ASSERT_TRUE(false) << "Iterator should be empty but elt found ";
    }
}

TEST(LWWMap, iteratorReferenceTest) {
    LWWMap<std::string, int, int> data0;

    // Add all
    data0.add("e1", 1);
    auto it = data0.begin();
    ASSERT_EQ(it->first, "e1");
}


// -----------------------------------------------------------------------------
// Operator==
// -----------------------------------------------------------------------------

TEST(LWWMap, operatorEQTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

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

TEST(LWWMap, operatorEQWithDifferentTimestampTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // Timestamp doesn't count from end user point of view
    data0.add("v1", 1);
    data1.add("v1", 2);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);
}

TEST(LWWMap, operatorEQWithDifferentValueTest) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // Setup data
    data0.add("v1", 1); // v1 == default set value
    data1.add("v1", 1); // v2 == default set value

    // Change value
    LWWMap<std::string, int, int>::iterator it0 = data0.find("v1");
    it0->second = 42; // v1 == 42

    ASSERT_TRUE(data0 != data1);
    ASSERT_FALSE(data0 == data1);

    // Restore to same value
    auto it_1 = data1.find("v1");
    it_1->second = 42; // v1 == 42
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);
}


}} // End namespaces


