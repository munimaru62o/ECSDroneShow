#include "pch.h"
#include "Engine/ECS/SystemManager.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

// ---------------------------------------------------------
// Dummy Systems and Globals for Testing
// ---------------------------------------------------------

// Global array to record the execution order of systems
static std::vector<int> g_executionOrder;

class DummySystemA : public System
{
public:
    void Init(Coordinator& c) override {}
    void Update(Coordinator& c, float dt, double simulationTime) override { g_executionOrder.push_back(1); }
};

class DummySystemB : public System
{
public:
    void Init(Coordinator& c) override {}
    void Update(Coordinator& c, float dt, double simulationTime) override { g_executionOrder.push_back(2); }
};

class DummyTrackingSystem : public System
{
public:
    void Init(Coordinator& c) override {}
    void Update(Coordinator& c, float dt, double simulationTime) override {}

    size_t GetEntityCount() const { return GetEntities().size(); }
    bool Contains(Entity e) const { return std::find(GetEntities().begin(), GetEntities().end(), e) != GetEntities().end(); }
};

// ---------------------------------------------------------
// 1. Execution Order Sorting Test
// ---------------------------------------------------------
TEST(SystemManagerTest, SystemOrderSorting)
{
    auto sysManager = std::make_unique<SystemManager>();
    Coordinator dummyCoordinator; // Dummy coordinator for InitSystems

    g_executionOrder.clear();

    // Intentionally register in reverse order (SystemB first, then SystemA).
    // However, assign a smaller (higher priority) order value to A(10) than B(20).
    sysManager->RegisterSystem<DummySystemB>(Signature{}, static_cast<SystemPhase>(0), 20);
    sysManager->RegisterSystem<DummySystemA>(Signature{}, static_cast<SystemPhase>(0), 10);

    // InitSystems triggers the internal sorting of the phase lists
    sysManager->InitSystems(dummyCoordinator);

    // Execute the update phase
    sysManager->UpdateSystems(static_cast<SystemPhase>(0), dummyCoordinator, 0.1f, 0.0f);

    // Expected result: Regardless of registration order, SystemA(1) should execute before SystemB(2) 
    // due to its lower order index.
    ASSERT_EQ(g_executionOrder.size(), 2);
    EXPECT_EQ(g_executionOrder[0], 1);
    EXPECT_EQ(g_executionOrder[1], 2);
}

// ---------------------------------------------------------
// 2. Entity Signature Matching Test
// ---------------------------------------------------------
TEST(SystemManagerTest, EntitySignatureChangedUpdatesSystemEntities)
{
    auto sysManager = std::make_unique<SystemManager>();

    // Define a required signature for the tracking system (e.g., requires Component ID 2 and 4)
    Signature requiredSignature;
    requiredSignature.set(2);
    requiredSignature.set(4);

    auto trackingSystem = sysManager->RegisterSystem<DummyTrackingSystem>(
        requiredSignature, static_cast<SystemPhase>(0), 0
    );

    Entity entity = 10;
    Signature entitySignature;

    // 1. Entity has no components matching the requirement
    sysManager->EntitySignatureChanged(entity, entitySignature);
    EXPECT_FALSE(trackingSystem->Contains(entity));

    // 2. Entity gains only one of the required components
    entitySignature.set(2);
    sysManager->EntitySignatureChanged(entity, entitySignature);
    EXPECT_FALSE(trackingSystem->Contains(entity));

    // 3. Entity gains all required components -> Should be added to the system
    entitySignature.set(4);
    sysManager->EntitySignatureChanged(entity, entitySignature);
    EXPECT_TRUE(trackingSystem->Contains(entity));

    // 4. Entity loses a required component -> Should be erased from the system
    entitySignature.reset(2);
    sysManager->EntitySignatureChanged(entity, entitySignature);
    EXPECT_FALSE(trackingSystem->Contains(entity));
}

// ---------------------------------------------------------
// 3. Entity Destruction Cleanup Test
// ---------------------------------------------------------
TEST(SystemManagerTest, EntityDestroyedRemovesFromAllSystems)
{
    auto sysManager = std::make_unique<SystemManager>();

    // System with no specific signature requirements (accepts anything with at least bit 0)
    Signature sig;
    sig.set(0);
    auto system = sysManager->RegisterSystem<DummyTrackingSystem>(sig, static_cast<SystemPhase>(0), 0);

    Entity entity = 5;

    // Manually add the entity to the system
    sysManager->EntitySignatureChanged(entity, sig);
    EXPECT_TRUE(system->Contains(entity));

    // Trigger destruction
    sysManager->EntityDestroyed(entity);

    // Verify it was correctly purged from the system's internal set
    EXPECT_FALSE(system->Contains(entity));
}

// ---------------------------------------------------------
// 4. System Retrieval Test
// ---------------------------------------------------------
TEST(SystemManagerTest, GetSystemReturnsCorrectInstance)
{
    auto sysManager = std::make_unique<SystemManager>();

    auto registeredPtr = sysManager->RegisterSystem<DummySystemA>(Signature{}, static_cast<SystemPhase>(0), 10);
    auto retrievedPtr = sysManager->GetSystem<DummySystemA>();

    // Ensure that GetSystem returns the exact same pointer allocated during registration
    EXPECT_NE(retrievedPtr, nullptr);
    EXPECT_EQ(registeredPtr, retrievedPtr);
}
