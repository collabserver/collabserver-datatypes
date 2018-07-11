#include "collabdata/custom/CollabData.h"

#include <gtest/gtest.h>

#include "collabdata/custom/Operation.h"
#include "collabdata/custom/OperationObserver.h"
#include "collabdata/custom/OperationVisitor.h"

namespace collab {


// -----------------------------------------------------------------------------
// Mock classes
// -----------------------------------------------------------------------------

static int nbNotified = 0; // Tracks nb of observer notified

// -----------------------------------------------------------------------------
class MockCollabData : public CollabData {
    public:
        bool applyExternOperation(const std::stringstream& buffer) override {
            return false;
        }
};

// -----------------------------------------------------------------------------
class MockOperationObserver : public OperationObserver {
    public:
        MockOperationObserver() = default;

    public:
        void onOperation(const Operation& op) override {
            nbNotified++;
        }
};

// -----------------------------------------------------------------------------
class MockOperation : public Operation {
    public:
        int getType() const override {
            return 1;
        }
        bool serialize(std::stringstream& buffer) const override {
            return false;
        }
        bool unserialize(const std::stringstream& buffer) override {
            return false;
        }
        void accept(OperationVisitor& visitor) const override {
        }
};


// -----------------------------------------------------------------------------
// addOperationObserver()
// -----------------------------------------------------------------------------

TEST(CollabData, addOperationObserverTest) {
    MockCollabData data;

    MockOperationObserver obs1;
    MockOperationObserver obs2;
    MockOperationObserver obs3;

    ASSERT_EQ(data.sizeOperationObserver(), 0);
    data.addOperationObserver(obs1);
    ASSERT_EQ(data.sizeOperationObserver(), 1);
    data.addOperationObserver(obs2);
    ASSERT_EQ(data.sizeOperationObserver(), 2);
    data.addOperationObserver(obs3);
    ASSERT_EQ(data.sizeOperationObserver(), 3);
}

TEST(CollabData, addOperationObserverTest_DuplicateCall) {
    MockCollabData data;

    MockOperationObserver obs1;
    MockOperationObserver obs2;
    MockOperationObserver obs3;

    ASSERT_EQ(data.sizeOperationObserver(), 0);
    data.addOperationObserver(obs1);
    data.addOperationObserver(obs1);
    data.addOperationObserver(obs1);
    ASSERT_EQ(data.sizeOperationObserver(), 1);
    data.addOperationObserver(obs2);
    data.addOperationObserver(obs2);
    ASSERT_EQ(data.sizeOperationObserver(), 2);
    data.addOperationObserver(obs3);
    data.addOperationObserver(obs3);
    data.addOperationObserver(obs3);
    ASSERT_EQ(data.sizeOperationObserver(), 3);
}

TEST(CollabData, addOperationObserverTest_ReturnType) {
    MockCollabData data;

    MockOperationObserver obs1;
    MockOperationObserver obs2;
    MockOperationObserver obs3;

    ASSERT_TRUE(data.addOperationObserver(obs1));
    ASSERT_TRUE(data.addOperationObserver(obs2));
    ASSERT_TRUE(data.addOperationObserver(obs3));

    ASSERT_FALSE(data.addOperationObserver(obs1));
    ASSERT_FALSE(data.addOperationObserver(obs2));
    ASSERT_FALSE(data.addOperationObserver(obs3));
}


// -----------------------------------------------------------------------------
// clearOperationObserver()
// -----------------------------------------------------------------------------

TEST(CollabData, clearOperationObserverTest) {
    MockCollabData data;

    MockOperationObserver obs1;
    MockOperationObserver obs2;
    MockOperationObserver obs3;

    ASSERT_EQ(data.sizeOperationObserver(), 0);
    data.addOperationObserver(obs1);
    data.addOperationObserver(obs2);
    data.addOperationObserver(obs3);
    ASSERT_EQ(data.sizeOperationObserver(), 3);

    data.clearOperationObservers();
    ASSERT_EQ(data.sizeOperationObserver(), 0);
}


// -----------------------------------------------------------------------------
// notifyOperationObserver()
// -----------------------------------------------------------------------------

TEST(CollabData, notifyOperationObserversTest) {
    MockCollabData data;

    MockOperation op;
    MockOperationObserver obs1;
    MockOperationObserver obs2;
    MockOperationObserver obs3;
    data.addOperationObserver(obs1);
    data.addOperationObserver(obs2);
    data.addOperationObserver(obs3);

    nbNotified = 0;
    data.notifyOperationObservers(op);
    ASSERT_EQ(nbNotified, 3);

    nbNotified = 0;
    data.notifyOperationObservers(op);
    ASSERT_EQ(nbNotified, 3);
}

TEST(CollabData, notifyOperationObserversTest_WithAddAndClear) {
    MockCollabData data;

    MockOperation op;
    MockOperationObserver obs1;
    MockOperationObserver obs2;
    MockOperationObserver obs3;

    // Notify with zero observers
    nbNotified = 0;
    data.notifyOperationObservers(op);
    ASSERT_EQ(nbNotified, 0);

    // Add observers and notify
    data.addOperationObserver(obs1);
    data.addOperationObserver(obs2);
    nbNotified = 0;
    data.notifyOperationObservers(op);
    ASSERT_EQ(nbNotified, 2);

    // Clear and notify
    data.clearOperationObservers();
    nbNotified = 0;
    data.notifyOperationObservers(op);
    ASSERT_EQ(nbNotified, 0);

    // Re-add and notify
    data.addOperationObserver(obs2);
    nbNotified = 0;
    data.notifyOperationObservers(op);
    ASSERT_EQ(nbNotified, 1);
}


// -----------------------------------------------------------------------------
// setOperationBroadcaster()
// -----------------------------------------------------------------------------

TEST(CollabData, setOperationBroadcasterTest) {
    MockCollabData data;
    MockOperationObserver broadcaster;
    ASSERT_FALSE(data.hasBroadcaster());
    data.setOperationBroadcaster(broadcaster);
    ASSERT_TRUE(data.hasBroadcaster());
}

TEST(CollabData, removeOperationBroadcasterTest) {
    MockCollabData data;
    MockOperationObserver broadcaster;
    ASSERT_FALSE(data.hasBroadcaster());
    data.setOperationBroadcaster(broadcaster);
    ASSERT_TRUE(data.hasBroadcaster());
    data.removeOperationBroadcaster();
    ASSERT_FALSE(data.hasBroadcaster());
}

TEST(CollabData, notifyOperationBroadcasterTest) {
    MockCollabData data;
    MockOperation op;
    MockOperationObserver broadcaster;
    data.setOperationBroadcaster(broadcaster);

    nbNotified = 0;
    data.notifyOperationBroadcaster(op);
    ASSERT_EQ(nbNotified, 1);
}


} // End namespace


