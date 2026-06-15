#include "pch.h"
#include "Engine/ECS/ComponentArray.h"

// Dummy component for testing purposes
struct DummyComponent
{
    int id;
    float value;
};

// ---------------------------------------------------------
// 1. Basic insertion and retrieval test
// ---------------------------------------------------------
TEST(ComponentArrayTest, InsertAndGetData)
{
    auto compArray = std::make_unique<ComponentArray<DummyComponent>>();
    Entity entity1 = 0;

    // Insert data
    compArray->InsertData(entity1, DummyComponent{ 10, 3.14f });

    // Retrieve and verify data
    auto& data = compArray->GetData(entity1);
    EXPECT_EQ(data.id, 10);
    EXPECT_FLOAT_EQ(data.value, 3.14f);

    // Verify that values can be correctly updated via reference
    data.id = 99;
    EXPECT_EQ(compArray->GetData(entity1).id, 99);
}

// ---------------------------------------------------------
// 2. Test to ensure Swap and Pop deletion works correctly
// ---------------------------------------------------------
TEST(ComponentArrayTest, RemoveDataMaintainsIntegrity)
{
    auto compArray = std::make_unique<ComponentArray<DummyComponent>>();
    Entity e1 = 1;
    Entity e2 = 2;
    Entity e3 = 3;

    compArray->InsertData(e1, DummyComponent{ 1, 10.0f });
    compArray->InsertData(e2, DummyComponent{ 2, 20.0f });
    compArray->InsertData(e3, DummyComponent{ 3, 30.0f });

    // Remove the middle entity (e2).
    // Behind the scenes, e3's data should be swapped into e2's position.
    compArray->RemoveData(e2);

    // Verify that the remaining data for e1 and e3 is intact and can be retrieved correctly.
    EXPECT_EQ(compArray->GetData(e1).id, 1);
    EXPECT_FLOAT_EQ(compArray->GetData(e1).value, 10.0f);

    EXPECT_EQ(compArray->GetData(e3).id, 3);
    EXPECT_FLOAT_EQ(compArray->GetData(e3).value, 30.0f);
}

// ---------------------------------------------------------
// 3. Test for destruction events via the interface
// ---------------------------------------------------------
TEST(ComponentArrayTest, EntityDestroyedCleansUp)
{
    auto compArray = std::make_unique<ComponentArray<DummyComponent>>();
    Entity e1 = 100;
    Entity e2 = 200;

    compArray->InsertData(e1, DummyComponent{ 1, 1.0f });

    // Delete using IComponentArray polymorphism
    IComponentArray* iArray = compArray.get();

    // Since it contains e1, the deletion process should run normally.
    iArray->EntityDestroyed(e1);

    // It does not contain e2, but it should be safely ignored by the 
    // internal if (m_entitySet.Has(entity)) check without crashing.
    iArray->EntityDestroyed(e2);

    // If it reaches this point without crashing, the test succeeds.
    SUCCEED();
}

// ---------------------------------------------------------
// 4. Test SetData and Update Event Tracking
// ---------------------------------------------------------
TEST(ComponentArrayTest, SetDataUpdatesValuesAndTriggersEvents)
{
    auto compArray = std::make_unique<ComponentArray<DummyComponent>>();
    Entity e1 = 5;

    compArray->InsertData(e1, DummyComponent{ 1, 10.0f });

    // Overwrite the existing data using SetData
    compArray->SetData(e1, DummyComponent{ 2, 20.0f });

    // Verify the data was successfully updated
    auto& data = compArray->GetData(e1);
    EXPECT_EQ(data.id, 2);
    EXPECT_FLOAT_EQ(data.value, 20.0f);

    // Verify that the update event was correctly registered
    const auto& events = compArray->GetEvents();
    EXPECT_TRUE(events.dirty.test(e1));
    ASSERT_EQ(events.updated.size(), 1);
    EXPECT_EQ(events.updated[0], e1);
}

// ---------------------------------------------------------
// 5. Test Added/Removed Events and ClearEvents functionality
// ---------------------------------------------------------
TEST(ComponentArrayTest, EventTrackingAndClearing)
{
    auto compArray = std::make_unique<ComponentArray<DummyComponent>>();
    Entity e1 = 10;
    Entity e2 = 20;

    // Trigger Add events
    compArray->InsertData(e1, DummyComponent{ 1, 1.0f });
    compArray->InsertData(e2, DummyComponent{ 2, 2.0f });

    const auto& events = compArray->GetEvents();
    EXPECT_EQ(events.added.size(), 2);
    EXPECT_EQ(events.added[0], e1);

    // Trigger Remove events
    compArray->RemoveData(e1);
    EXPECT_EQ(events.removed.size(), 1);
    EXPECT_EQ(events.removed[0], e1);

    // Trigger Update events for the remaining entity
    compArray->SetData(e2, DummyComponent{ 99, 99.0f });
    EXPECT_TRUE(events.dirty.test(e2));

    // Clear all events
    compArray->ClearEvents();

    // Verify that all event containers and bitsets are completely reset
    EXPECT_TRUE(events.added.empty());
    EXPECT_TRUE(events.removed.empty());
    EXPECT_TRUE(events.updated.empty());
    EXPECT_FALSE(events.dirty.test(e2));
}
