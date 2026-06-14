#include "pch.h"
#include "Engine/ECS/ComponentManager.h"

// Dummy components for testing purposes
struct DummyGravityComponent
{
    float force;
};

struct RigidBodyComponent
{
    float velocityX;
    float velocityY;
};

// ---------------------------------------------------------
// 1. Component Registration and ID Issuance Test
// ---------------------------------------------------------
TEST(ComponentManagerTest, RegisterAndGetComponentType)
{
    ComponentManager cm;

    // Registration process (internally instantiates ComponentArrays)
    cm.RegisterComponent<DummyGravityComponent>();
    cm.RegisterComponent<RigidBodyComponent>();

    // Verify that distinct component types are always assigned different IDs
    ComponentType typeA = cm.GetComponentType<DummyGravityComponent>();
    ComponentType typeB = cm.GetComponentType<RigidBodyComponent>();

    EXPECT_NE(typeA, typeB);
}

// ---------------------------------------------------------
// 2. Component Addition and Retrieval Test
// ---------------------------------------------------------
TEST(ComponentManagerTest, AddAndGetComponent)
{
    ComponentManager cm;
    cm.RegisterComponent<DummyGravityComponent>();

    Entity entity = 5;

    // Add a component
    cm.AddComponent(entity, DummyGravityComponent{ 9.8f });

    // Retrieve and verify
    auto& grav = cm.GetComponent<DummyGravityComponent>(entity);
    EXPECT_FLOAT_EQ(grav.force, 9.8f);

    // Verify that the value can be successfully updated via reference
    grav.force = 1.62f;
    EXPECT_FLOAT_EQ(cm.GetComponent<DummyGravityComponent>(entity).force, 1.62f);
}

// ---------------------------------------------------------
// 3. Entity Destruction Cleanup Linked Test
// ---------------------------------------------------------
TEST(ComponentManagerTest, EntityDestroyedCleansUpAllArrays)
{
    ComponentManager cm;
    cm.RegisterComponent<DummyGravityComponent>();
    cm.RegisterComponent<RigidBodyComponent>();

    Entity e1 = 10;
    Entity e2 = 20;

    // Assign two components to e1
    cm.AddComponent(e1, DummyGravityComponent{ 9.8f });
    cm.AddComponent(e1, RigidBodyComponent{ 1.0f, 0.0f });

    // Assign only one component to e2
    cm.AddComponent(e2, DummyGravityComponent{ 5.0f });

    // Destroy e1 (Data for e1 should be wiped from all associated ComponentArrays)
    Signature sig;
    sig.set(cm.GetComponentType<DummyGravityComponent>());
    sig.set(cm.GetComponentType<RigidBodyComponent>());
    cm.EntityDestroyed(e1, sig);

    // Verify that e2's data remains completely unaffected and intact
    EXPECT_FLOAT_EQ(cm.GetComponent<DummyGravityComponent>(e2).force, 5.0f);
}

// ---------------------------------------------------------
// 4. Explicit Component Removal Test
// ---------------------------------------------------------
TEST(ComponentManagerTest, RemoveComponentWorksCorrectly)
{
    ComponentManager cm;
    cm.RegisterComponent<DummyGravityComponent>();

    Entity entity = 7;
    cm.AddComponent(entity, DummyGravityComponent{ 9.8f });

    // Remove the component explicitly
    cm.RemoveComponent<DummyGravityComponent>(entity);

    // Verify removal via the event system (since direct HasComponent is typically in Coordinator)
    const auto& events = cm.GetEvents<DummyGravityComponent>();
    ASSERT_EQ(events.removed.size(), 1);
    EXPECT_EQ(events.removed[0], entity);
}

// ---------------------------------------------------------
// 5. Event Tracking via SetComponent Test
// ---------------------------------------------------------
TEST(ComponentManagerTest, SetComponentUpdatesValueAndTriggersEvent)
{
    ComponentManager cm;
    cm.RegisterComponent<DummyGravityComponent>();

    Entity entity = 8;
    cm.AddComponent(entity, DummyGravityComponent{ 9.8f });

    // Overwrite data using SetComponent (should trigger update event)
    cm.SetComponent(entity, DummyGravityComponent{ 3.7f }); // e.g., Mars gravity

    // Verify the data was updated
    auto& grav = cm.GetComponent<DummyGravityComponent>(entity);
    EXPECT_FLOAT_EQ(grav.force, 3.7f);

    // Verify that the dirty flag was properly set
    const auto& events = cm.GetEvents<DummyGravityComponent>();
    EXPECT_TRUE(events.dirty.test(entity));
    ASSERT_EQ(events.updated.size(), 1);
    EXPECT_EQ(events.updated[0], entity);
}

// ---------------------------------------------------------
// 6. Global ClearEvents Test
// ---------------------------------------------------------
TEST(ComponentManagerTest, GlobalClearEventsResetsAllRegisteredArrays)
{
    ComponentManager cm;
    cm.RegisterComponent<DummyGravityComponent>();
    cm.RegisterComponent<RigidBodyComponent>();

    Entity entity = 9;
    cm.AddComponent(entity, DummyGravityComponent{ 9.8f });
    cm.AddComponent(entity, RigidBodyComponent{ 10.0f, 0.0f });

    // Ensure events were recorded
    EXPECT_FALSE(cm.GetEvents<DummyGravityComponent>().added.empty());
    EXPECT_FALSE(cm.GetEvents<RigidBodyComponent>().added.empty());

    // Call the global ClearEvents (no template arguments)
    cm.ClearEvents();

    // Verify that all underlying ComponentArrays have their events cleared
    EXPECT_TRUE(cm.GetEvents<DummyGravityComponent>().added.empty());
    EXPECT_TRUE(cm.GetEvents<RigidBodyComponent>().added.empty());
}
