#include <gtest/gtest.h>

#include "collabserver/data/CmRDT/LWWMap.h"

// Check the whole internal state of an element
#define _ASSERT_ELT_EQ(elt_it_, key_, is_removed_, stamp_, data_) \
    ASSERT_TRUE(elt_it_ != data_.crdt_end());                     \
    EXPECT_EQ(elt_it_->first, key_);                              \
    EXPECT_EQ(elt_it_->second.isRemoved(), is_removed_);          \
    EXPECT_EQ(elt_it_->second.timestamp(), stamp_)

namespace collabserver {

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

TEST(LWWMap, crdtEmptyTest) {
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

TEST(LWWMap, sizeTest_WithDuplicateAdd) {
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

TEST(LWWMap, sizeTest_WithDuplicateAddRemove) {
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

TEST(LWWMap, sizeTest_WithRemoveFirst) {
    LWWMap<int, int, int> data0;
    ASSERT_EQ(data0.size(), 0);

    data0.remove(1, 22);
    data0.remove(1, 28);
    data0.remove(1, 29);
    data0.remove(1, 25);
    data0.remove(1, 21);
    ASSERT_EQ(data0.size(), 0);  // 0, not -5 or shit

    data0.add(1, 10);
    ASSERT_EQ(data0.size(), 0);

    data0.add(1, 30);
    ASSERT_EQ(data0.size(), 1);

    data0.remove(2, 10);
    data0.remove(2, 45);
    data0.remove(2, 40);
    ASSERT_EQ(data0.size(), 1);
}

TEST(LWWMap, sizeTest_WithOlderRemoveAfterAdd) {
    LWWMap<int, int, int> data0;
    ASSERT_EQ(data0.size(), 0);

    data0.remove(1, 10);
    ASSERT_EQ(data0.size(), 0);
    data0.add(1, 20);
    ASSERT_EQ(data0.size(), 1);
    data0.remove(1, 11);
    data0.remove(1, 12);
    data0.remove(1, 18);
    ASSERT_EQ(data0.size(), 1);  // Not removed (See timestamps)
}

// -----------------------------------------------------------------------------
// max_size()
// -----------------------------------------------------------------------------

TEST(LWWMap, maxSizeTest) {
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

TEST(LWWMap, countTest_AfterRemove) {
    LWWMap<int, int, int> data0;

    ASSERT_EQ(data0.count(42), 0);
    data0.add(42, 10);
    data0.remove(42, 20);
    ASSERT_EQ(data0.count(42), 0);
}

// -----------------------------------------------------------------------------
// crdt_count()
// -----------------------------------------------------------------------------
TEST(LWWMap, crdtCountTest) {
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

TEST(LWWMap, crdtCountTest_AfterRemove) {
    LWWMap<int, int, int> data0;

    ASSERT_EQ(data0.crdt_count(42), 0);
    data0.add(42, 10);
    data0.remove(42, 20);
    ASSERT_EQ(data0.crdt_count(42), 1);
}

// -----------------------------------------------------------------------------
// at()
// -----------------------------------------------------------------------------

TEST(LWWMap, atTest) {
    LWWMap<const char*, const char*, int> data0;

    // Add all the crap there!
    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);

    // Set data
    data0.at("e1") = "Carrot";
    data0.at("e2") = "SuperRabbit";
    data0.at("e3") = "MagicRabbit";

    // Find them all!
    EXPECT_EQ(data0.at("e1"), "Carrot");
    EXPECT_EQ(data0.at("e2"), "SuperRabbit");
    EXPECT_EQ(data0.at("e3"), "MagicRabbit");
}

TEST(LWWMap, atTest_RemovedElement) {
    LWWMap<std::string, const char*, int> data0;

    // Setup data
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);

    data0.at("e1") = "value_1";
    data0.at("e2") = "value_2";
    data0.at("e3") = "value_3";

    data0.remove("e1", 20);
    data0.remove("e2", 20);

    // Check at() exceptions
    int nbException = 0;
    const char* values[3] = {"e1", "e2", "e3"};
    for (int k = 0; k < 3; ++k) {
        try {
            data0.at(values[k]);
        } catch (std::out_of_range e) {
            nbException++;
        }
    }
    ASSERT_EQ(nbException, 2);
}

TEST(LWWMap, atTest_ChangeValue) {
    LWWMap<std::string, int, int> data0;
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);

    // Set values
    data0.at("e1") = 1;
    data0.at("e2") = 2;
    data0.at("e3") = 3;
    ASSERT_EQ(data0.at("e1"), 1);
    ASSERT_EQ(data0.at("e2"), 2);
    ASSERT_EQ(data0.at("e3"), 3);

    // Change values
    data0.at("e1") = 32;
    data0.at("e2") = 64;
    data0.at("e3") = 128;
    ASSERT_EQ(data0.at("e1"), 32);
    ASSERT_EQ(data0.at("e2"), 64);
    ASSERT_EQ(data0.at("e3"), 128);
}

TEST(LWWMap, atTest_InvalidKeyThrowException) {
    LWWMap<const char*, const char*, int> data0;

    int nbException = 0;
    const char* values[5] = {"e1", "e2", "e3", "carrot", "SuperRabbit"};
    for (int k = 0; k < 5; ++k) {
        try {
            data0.at(values[k]);
        } catch (std::out_of_range e) {
            nbException++;
        }
    }
    ASSERT_EQ(nbException, 5);
}

// -----------------------------------------------------------------------------
// crdt_at()
// -----------------------------------------------------------------------------

TEST(LWWMap, crdtAtTest) {
    LWWMap<const char*, const char*, int> data0;

    // Add all the crap there!
    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);

    // Set data
    data0.crdt_at("e1") = "Carrot";
    data0.crdt_at("e2") = "SuperRabbit";
    data0.crdt_at("e3") = "MagicRabbit";

    // Find them all!
    EXPECT_EQ(data0.crdt_at("e1"), "Carrot");
    EXPECT_EQ(data0.crdt_at("e2"), "SuperRabbit");
    EXPECT_EQ(data0.crdt_at("e3"), "MagicRabbit");
}

TEST(LWWMap, crdtAtTest_RemovedElement) {
    LWWMap<std::string, const char*, int> data0;

    // Setup data
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);

    data0.crdt_at("e1") = "value_1";
    data0.crdt_at("e2") = "value_2";
    data0.crdt_at("e3") = "value_3";

    data0.remove("e1", 20);
    data0.remove("e2", 20);

    // Check.crdt_at() exceptions
    int nbException = 0;
    const char* values[3] = {"e1", "e2", "e3"};
    for (int k = 0; k < 3; ++k) {
        try {
            data0.crdt_at(values[k]);
        } catch (std::out_of_range e) {
            nbException++;
        }
    }
    ASSERT_EQ(nbException, 0);
}

TEST(LWWMap, crdtAtTest_ChangeValue) {
    LWWMap<std::string, int, int> data0;
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);

    // Set values
    data0.crdt_at("e1") = 1;
    data0.crdt_at("e2") = 2;
    data0.crdt_at("e3") = 3;
    ASSERT_EQ(data0.crdt_at("e1"), 1);
    ASSERT_EQ(data0.crdt_at("e2"), 2);
    ASSERT_EQ(data0.crdt_at("e3"), 3);

    // Change values
    data0.crdt_at("e1") = 32;
    data0.crdt_at("e2") = 64;
    data0.crdt_at("e3") = 128;
    ASSERT_EQ(data0.crdt_at("e1"), 32);
    ASSERT_EQ(data0.crdt_at("e2"), 64);
    ASSERT_EQ(data0.crdt_at("e3"), 128);
}

TEST(LWWMap, crdtAtTest_InvalidKeyThrowException) {
    LWWMap<const char*, const char*, int> data0;

    int nbException = 0;
    const char* values[5] = {"e1", "e2", "e3", "carrot", "SuperRabbit"};
    for (int k = 0; k < 5; ++k) {
        try {
            data0.crdt_at(values[k]);
        } catch (std::out_of_range e) {
            nbException++;
        }
    }
    ASSERT_EQ(nbException, 5);
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

TEST(LWWMap, findTest_RemovedElement) {
    LWWMap<std::string, int, int> data0;

    data0.add("e1", 10);
    data0.remove("e1", 20);
    auto e1 = data0.find("e1");
    EXPECT_EQ(e1, data0.end());
}

TEST(LWWMap, findTest_AndChangeValue) {
    LWWMap<std::string, int, int> data0;

    // Setup data
    data0.add("e1", 1);  // v1 == default set value
    data0.add("e2", 2);  // v2 == default set value

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
// crdt_find()
// -----------------------------------------------------------------------------

TEST(LWWMap, crdtFindTest) {
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

TEST(LWWMap, findTest_QueryAndChangeValue) {
    LWWMap<std::string, int, int> data0;

    // Setup data
    data0.add("e1", 1);
    data0.add("e2", 2);
    data0.remove("e1", 3);
    data0.remove("e2", 3);

    // Change values of container
    auto e1_it = data0.crdt_find("e1");
    auto e2_it = data0.crdt_find("e2");
    e1_it->second.value() = 42;
    e2_it->second.value() = 1024;

    // Check if container value are well changed
    e1_it = data0.crdt_find("e1");
    e2_it = data0.crdt_find("e2");
    EXPECT_EQ(e1_it->second.value(), 42);
    EXPECT_EQ(e2_it->second.value(), 1024);
}

// -----------------------------------------------------------------------------
// clear()
// -----------------------------------------------------------------------------

TEST(LWWMap, clearTest) {
    LWWMap<std::string, int, int> data0;

    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);
    data0.add("e4", 14);
    data0.add("e5", 15);

    EXPECT_EQ(data0.size(), 5);
    EXPECT_EQ(data0.crdt_size(), 5);
    data0.clear(30);

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 5);
}

TEST(LWWMap, clearTest_CalledFirst) {
    LWWMap<std::string, int, int> data0;

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 0);

    // Older clear called first
    data0.clear(10);

    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);
    data0.add("e4", 14);
    data0.add("e5", 15);

    EXPECT_EQ(data0.size(), 5);
    EXPECT_EQ(data0.crdt_size(), 5);

    // Newer clear called first
    data0.clear(30);
    data0.add("e6", 21);
    data0.add("e7", 22);
    data0.add("e8", 23);
    data0.add("e9", 24);
    data0.add("e10", 25);

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 10);
}

TEST(LWWMap, clearTest_Indenpotent) {
    LWWMap<std::string, int, int> data0;

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 0);

    // Older clear called first
    data0.clear(10);
    data0.clear(10);

    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);
    data0.add("e4", 14);
    data0.add("e5", 15);

    EXPECT_EQ(data0.size(), 5);
    EXPECT_EQ(data0.crdt_size(), 5);

    // indenpotent
    data0.clear(10);
    data0.clear(10);
    data0.clear(10);
    data0.clear(10);

    EXPECT_EQ(data0.size(), 5);
    EXPECT_EQ(data0.crdt_size(), 5);

    // Newer clear called first
    data0.clear(30);
    data0.clear(30);
    data0.clear(30);

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 5);

    data0.add("e6", 21);
    data0.add("e7", 22);
    data0.add("e8", 23);
    data0.add("e9", 24);
    data0.add("e10", 25);

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 10);

    // indenpotent
    data0.clear(30);
    data0.clear(30);
    data0.clear(30);

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 10);
}

TEST(LWWMap, clearTest_IndenpotentReturnType) {
    LWWMap<std::string, int, int> data0;

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 0);

    // Clear at 42
    EXPECT_TRUE(data0.clear(42));
    EXPECT_FALSE(data0.clear(42));
    EXPECT_FALSE(data0.clear(42));

    // Clear at 20
    EXPECT_FALSE(data0.clear(20));
    EXPECT_FALSE(data0.clear(20));
    EXPECT_FALSE(data0.clear(20));

    // Clear at 64
    EXPECT_TRUE(data0.clear(64));
    EXPECT_FALSE(data0.clear(64));
    EXPECT_FALSE(data0.clear(64));
}

TEST(LWWMap, clearTest_ReturnType) {
    LWWMap<std::string, int, int> data0;
    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 0);

    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);
    EXPECT_TRUE(data0.clear(10));
    EXPECT_EQ(data0.size(), 3);  // Note: See clear() doc to understand '3'
    EXPECT_EQ(data0.crdt_size(), 3);

    // Duplicate clear later (Is applied)
    EXPECT_TRUE(data0.clear(20));
    EXPECT_EQ(data0.size(), 0);  // Note: See clear() doc to understand '3'
    EXPECT_EQ(data0.crdt_size(), 3);

    // Duplicate calls (Earlier. Not applied)
    EXPECT_FALSE(data0.clear(11));
    EXPECT_FALSE(data0.clear(12));
    EXPECT_FALSE(data0.clear(13));
    EXPECT_FALSE(data0.clear(14));

    EXPECT_EQ(data0.size(), 0);  // Note: See clear() doc to understand '3'
    EXPECT_EQ(data0.crdt_size(), 3);
}

TEST(LWWMap, clearTest_ThenAddAfterNewerClear) {
    LWWMap<std::string, int, int> data0;

    data0.clear(42);

    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);

    EXPECT_EQ(data0.size(), 0);
    EXPECT_EQ(data0.crdt_size(), 3);
}

TEST(LWWMap, clearTest_ThenAddAfterOlderClear) {
    LWWMap<std::string, int, int> data0;

    data0.clear(10);

    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);

    EXPECT_EQ(data0.size(), 3);
    EXPECT_EQ(data0.crdt_size(), 3);
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
    for (int k = 0; k < 4; ++k) {
        auto coco = data0.crdt_find(k);
        _ASSERT_ELT_EQ(coco, k, false, 10, data0);
    }

    // More add later
    data0.add(0, 20);
    data0.add(1, 20);
    data0.add(2, 20);
    data0.add(3, 20);
    for (int k = 0; k < 4; ++k) {
        auto coco = data0.crdt_find(k);
        _ASSERT_ELT_EQ(coco, k, false, 20, data0);
    }
}

TEST(LWWMap, addTest_DuplicateCalls) {
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

TEST(LWWMap, addTest_ReturnType) {
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

TEST(LWWMap, addTest_ReturnTypeWithRemoveCalled) {
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

TEST(LWWMap, addTest_Idempotent) {
    LWWMap<std::string, int, int> data0;

    data0.add("e1", 10);
    data0.add("e1", 10);
    data0.add("e1", 10);
    data0.add("e1", 10);
    data0.add("e1", 10);

    ASSERT_EQ(data0.size(), 1);
    ASSERT_EQ(data0.crdt_size(), 1);

    auto coco = data0.crdt_find("e1");
    _ASSERT_ELT_EQ(coco, "e1", false, 10, data0);
}

TEST(LWWMap, addTest_IdempotentReturnType) {
    LWWMap<std::string, int, int> data0;

    ASSERT_TRUE(data0.add("e1", 10));
    ASSERT_FALSE(data0.add("e1", 10));
    ASSERT_FALSE(data0.add("e1", 10));
    ASSERT_FALSE(data0.add("e1", 10));
    ASSERT_FALSE(data0.add("e1", 10));

    ASSERT_EQ(data0.size(), 1);
    ASSERT_EQ(data0.crdt_size(), 1);

    auto coco = data0.crdt_find("e1");
    _ASSERT_ELT_EQ(coco, "e1", false, 10, data0);
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
    for (int k = 0; k < 4; ++k) {
        auto res = data0.crdt_find(k);
        _ASSERT_ELT_EQ(res, k, true, 20, data0);
    }
}

TEST(LWWMap, removeTest_DuplicateCalls) {
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

TEST(LWWMap, removeTest_CalledBeforeAddCall) {
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

TEST(LWWMap, removeTest_CalledFirstReturnType) {
    LWWMap<std::string, int, int> data0;

    ASSERT_FALSE(data0.remove("coco", 20));
    ASSERT_FALSE(data0.remove("coco", 10));
    ASSERT_FALSE(data0.remove("coco", 30));

    data0.add("coco", 15);
    ASSERT_FALSE(data0.remove("coco", 40));

    data0.add("coco", 80);
    ASSERT_TRUE(data0.remove("coco", 90));
}

TEST(LWWMap, removeTest_ReturnType) {
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
TEST(LWWMap, removeTest_Idempotent) {
    LWWMap<std::string, int, int> data0;

    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);
    data0.remove("e1", 20);
    data0.remove("e1", 20);
    data0.remove("e1", 20);
    data0.remove("e1", 20);

    ASSERT_EQ(data0.size(), 2);
    ASSERT_EQ(data0.crdt_size(), 3);

    auto coco = data0.crdt_find("e1");
    _ASSERT_ELT_EQ(coco, "e1", true, 20, data0);
}

TEST(LWWMap, removeTest_IdempotentReturnType) {
    LWWMap<std::string, int, int> data0;

    data0.add("e1", 11);
    data0.add("e2", 12);
    data0.add("e3", 13);
    ASSERT_TRUE(data0.remove("e1", 20));
    ASSERT_FALSE(data0.remove("e1", 20));
    ASSERT_FALSE(data0.remove("e1", 20));
    ASSERT_FALSE(data0.remove("e1", 20));

    ASSERT_EQ(data0.size(), 2);
    ASSERT_EQ(data0.crdt_size(), 3);

    auto coco = data0.crdt_find("e1");
    _ASSERT_ELT_EQ(coco, "e1", true, 20, data0);
}

// -----------------------------------------------------------------------------
// add() + remove()
// -----------------------------------------------------------------------------

TEST(LWWMap, addTest_ConcurrentWithRemove) {
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

TEST(LWWMap, addTest_ConcurrentRemoveWithRemoveCalledFirst) {
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

TEST(LWWMap, addTest_ConcurrentRemoveUseCase) {
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

TEST(LWWMap, crdtSizeTest) {
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

TEST(LWWMap, crdtEqualTest_WithOnlyAdd) {
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

TEST(LWWMap, crdtEqualTest_WithAddRemove) {
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

TEST(LWWMap, crdtEqualTest_SameValueButDifferentTimestamp) {
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

TEST(LWWMap, crdtEqualTest_WithUsersameButInternalNotSame) {
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

TEST(LWWMap, crdtEqualTest_EmptyVsAdd) {
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

TEST(LWWMap, operatorEQTest_WithDifferentTimestamp) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // Timestamp doesn't count from end user point of view
    data0.add("v1", 1);
    data1.add("v1", 2);
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);
}

TEST(LWWMap, operatorEQTest_WithDifferentValue) {
    LWWMap<std::string, int, int> data0;
    LWWMap<std::string, int, int> data1;

    // Setup data
    data0.add("v1", 1);  // v1 == default set value
    data1.add("v1", 1);  // v2 == default set value

    // Change value
    LWWMap<std::string, int, int>::iterator it0 = data0.find("v1");
    it0->second = 42;  // v1 == 42

    ASSERT_TRUE(data0 != data1);
    ASSERT_FALSE(data0 == data1);

    // Restore to same value
    auto it_1 = data1.find("v1");
    it_1->second = 42;  // v1 == 42
    ASSERT_TRUE(data0 == data1);
    ASSERT_FALSE(data0 != data1);
}

// -----------------------------------------------------------------------------
// LWWMap::iterator
// -----------------------------------------------------------------------------

TEST(LWWMap, iteratorTest) {
    LWWMap<const char*, int, int> data0;

    data0.add("e0", 10);
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);

    data0.at("e0") = 0;
    data0.at("e1") = 100;
    data0.at("e2") = 200;
    data0.at("e3") = 300;

    int k = 0;
    int total = 0;
    for (auto it = data0.begin(); it != data0.end(); ++it) {
        ++k;
        total += it->second;
    }
    EXPECT_EQ(k, 4);
    EXPECT_EQ(total, 600);

    // Remove elements, then iterator should not use them
    data0.remove("e1", 20);
    data0.remove("e2", 20);
    k = 0;
    total = 0;
    for (auto& elt : data0) {
        ++k;
        total += elt.second;
    }
    EXPECT_EQ(k, 2);
    EXPECT_EQ(total, 300);

    // Add again some more and test
    data0.add("e4", 30);
    data0.add("e5", 30);
    data0.add("e6", 30);
    data0.add("e7", 30);
    data0.at("e4") = 400;
    data0.at("e5") = 500;
    data0.at("e6") = 600;
    data0.at("e7") = 700;
    k = 0;
    total = 0;
    for (auto it = data0.begin(); it != data0.end(); ++it) {
        ++k;
        total += it->second;
    }
    EXPECT_EQ(k, 6);
    EXPECT_EQ(total, 2500);
}

TEST(LWWMap, iteratorTest_EmptyMap) {
    LWWMap<int, int, int> data0;

    int k = 0;
    // If you wonder: LWWMap<int,int,int>::iterator it = data0.begin();
    for (auto it = data0.begin(); it != data0.end(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";
}

TEST(LWWMap, iteratorTest_EmptyMapAfterAddRemove) {
    LWWMap<int, int, int> data0;

    data0.add(0, 10);
    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);
    data0.add(4, 10);
    data0.remove(0, 20);
    data0.remove(1, 20);
    data0.remove(2, 20);
    data0.remove(3, 20);
    data0.remove(4, 20);
    int k = 0;
    for (auto it = data0.begin(); it != data0.end(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";

    // Add more and remove
    data0.add(5, 30);
    data0.remove(5, 31);
    k = 0;
    for (auto it = data0.begin(); it != data0.end(); ++it) {
        ++k;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";

    // Add / remove again
    data0.add(6, 40);
    data0.add(7, 41);
    data0.add(8, 42);
    data0.add(9, 43);
    data0.remove(6, 44);
    data0.remove(7, 45);
    data0.remove(8, 46);
    data0.remove(9, 47);
    k = 0;
    for (auto it = data0.begin(); it != data0.end(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";
}

TEST(LWWMap, iteratorTest_Reference) {
    LWWMap<std::string, int, int> data0;

    // Add all
    data0.add("e1", 1);
    auto it = data0.begin();
    ASSERT_EQ(it->first, "e1");
}

TEST(LWWMap, iteratorTest_end) {
    LWWMap<std::string, int, int> data0;

    int k = 0;
    for (auto it = data0.end(); it != data0.end(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0);

    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);
    k = 0;
    for (auto it = data0.end(); it != data0.end(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0);
}

// -----------------------------------------------------------------------------
// LWWMap::const_iterator
// -----------------------------------------------------------------------------

TEST(LWWMap, constIteratorTest) {
    LWWMap<const char*, int, int> data0;

    data0.add("e0", 10);
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);

    data0.at("e0") = 0;
    data0.at("e1") = 100;
    data0.at("e2") = 200;
    data0.at("e3") = 300;

    int k = 0;
    int total = 0;
    for (auto it = data0.cbegin(); it != data0.cend(); ++it) {
        ++k;
        total += it->second;
    }
    EXPECT_EQ(k, 4);
    EXPECT_EQ(total, 600);

    // Remove elements, then iterator should not use them
    data0.remove("e1", 20);
    data0.remove("e2", 20);
    k = 0;
    total = 0;
    for (const auto& elt : data0) {
        ++k;
        total += elt.second;
    }
    EXPECT_EQ(k, 2);
    EXPECT_EQ(total, 300);

    // Add again some more and test
    data0.add("e4", 30);
    data0.add("e5", 30);
    data0.add("e6", 30);
    data0.add("e7", 30);
    data0.at("e4") = 400;
    data0.at("e5") = 500;
    data0.at("e6") = 600;
    data0.at("e7") = 700;
    k = 0;
    total = 0;
    for (auto it = data0.cbegin(); it != data0.cend(); ++it) {
        ++k;
        total += it->second;
    }
    EXPECT_EQ(k, 6);
    EXPECT_EQ(total, 2500);
}

TEST(LWWMap, constIteratorTest_EmptyMap) {
    LWWMap<int, int, int> data0;

    int k = 0;
    for (auto it = data0.cbegin(); it != data0.cend(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";
}

TEST(LWWMap, constIteratorTest_EmptyMapAfterAddRemove) {
    LWWMap<int, int, int> data0;

    data0.add(0, 10);
    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);
    data0.add(4, 10);
    data0.remove(0, 20);
    data0.remove(1, 20);
    data0.remove(2, 20);
    data0.remove(3, 20);
    data0.remove(4, 20);
    int k = 0;
    for (auto it = data0.cbegin(); it != data0.cend(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";

    // Add more and remove
    data0.add(5, 30);
    data0.remove(5, 31);
    k = 0;
    for (auto it = data0.cbegin(); it != data0.cend(); ++it) {
        ++k;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";

    // Add / remove again
    data0.add(6, 40);
    data0.add(7, 41);
    data0.add(8, 42);
    data0.add(9, 43);
    data0.remove(6, 44);
    data0.remove(7, 45);
    data0.remove(8, 46);
    data0.remove(9, 47);
    k = 0;
    for (auto it = data0.cbegin(); it != data0.cend(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";
}

TEST(LWWMap, constIteratorTest_Reference) {
    LWWMap<std::string, int, int> data0;

    // Add all
    data0.add("e1", 1);
    auto it = data0.cbegin();
    ASSERT_EQ(it->first, "e1");
}

TEST(LWWMap, constIteratorTest_end) {
    LWWMap<std::string, int, int> data0;

    int k = 0;
    for (auto it = data0.cend(); it != data0.cend(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0);

    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);
    k = 0;
    for (auto it = data0.cend(); it != data0.cend(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0);
}

// -----------------------------------------------------------------------------
// LWWMap::crdt_iterator
// -----------------------------------------------------------------------------

TEST(LWWMap, crdtIteratorTest) {
    LWWMap<const char*, int, int> data0;

    data0.add("e0", 10);
    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);

    data0.at("e0") = 0;
    data0.at("e1") = 100;
    data0.at("e2") = 200;
    data0.at("e3") = 300;

    int k = 0;
    int total = 0;
    for (auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ++k;
        total += it->second.value();
    }
    EXPECT_EQ(k, 4);
    EXPECT_EQ(total, 600);

    // Remove elements
    data0.remove("e1", 20);
    data0.remove("e2", 20);
    k = 0;
    total = 0;
    for (auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ++k;
        total += it->second.value();
    }
    EXPECT_EQ(k, 4);
    EXPECT_EQ(total, 600);

    // Add again some more and test
    data0.add("e4", 30);
    data0.add("e5", 30);
    data0.add("e6", 30);
    data0.add("e7", 30);
    data0.at("e4") = 400;
    data0.at("e5") = 500;
    data0.at("e6") = 600;
    data0.at("e7") = 700;
    k = 0;
    total = 0;
    for (auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        ++k;
        total += it->second.value();
    }
    EXPECT_EQ(k, 8);
    EXPECT_EQ(total, 2800);
}

TEST(LWWMap, crdtIteratorTest_EmptyMap) {
    LWWMap<int, int, int> data0;

    int k = 0;
    for (auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0) << "Iterator should be empty";
}

TEST(LWWMap, crdtIteratorTest_EmptyMapAfterAddRemove) {
    LWWMap<int, int, int> data0;

    data0.add(0, 10);
    data0.add(1, 10);
    data0.add(2, 10);
    data0.add(3, 10);
    data0.add(4, 10);

    data0.at(0) = 0;
    data0.at(1) = 100;
    data0.at(2) = 200;
    data0.at(3) = 300;
    data0.at(4) = 400;

    data0.remove(0, 20);
    data0.remove(1, 20);
    data0.remove(2, 20);
    data0.remove(3, 20);
    data0.remove(4, 20);

    int k = 0;
    int total = 0;
    for (auto it = data0.crdt_begin(); it != data0.crdt_end(); ++it) {
        k++;
        total += it->second.value();
    }
    EXPECT_EQ(k, 5) << "Iterator should be empty";
    EXPECT_EQ(total, 1000);
}

TEST(LWWMap, crdtIteratorTest_Reference) {
    LWWMap<std::string, int, int> data0;

    // Add all
    data0.add("e1", 1);
    auto it = data0.crdt_begin();
    ASSERT_EQ(it->first, "e1");
}

TEST(LWWMap, crdtIteratorTest_end) {
    LWWMap<std::string, int, int> data0;

    int k = 0;
    for (auto it = data0.crdt_end(); it != data0.crdt_end(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0);

    data0.add("e1", 10);
    data0.add("e2", 10);
    data0.add("e3", 10);
    k = 0;
    for (auto it = data0.crdt_end(); it != data0.crdt_end(); ++it) {
        k++;
    }
    EXPECT_EQ(k, 0);
}

}  // namespace collabserver
