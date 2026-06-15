#include "pch.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/System.h"
#include "Engine/Traits/ComponentTraits.h"

// ---------------------------------------------------------
// Dummy Data for Testing
// ---------------------------------------------------------
struct TestHealthComponent
{
    int hp;
};

struct TestPositionComponent
{
    float x, y;
};

// Dummy Core Component to test automatic attachment
struct TestCoreComponent
{
    bool isInitialized = false;
};

// Trait specialization to mark TestCoreComponent as a mandatory core component
template<>
struct ComponentTraits<TestCoreComponent>
{
    static constexpr bool CoreComponent = true;
};

// Dummy system to test Coordinator's System registration and signature matching
class TestHealthSystem : public System
{
public:
    void Init(Coordinator& coordinator) override {}
    void Update(Coordinator& coordinator, float dt, double simulationTime) override {}

    // Helper to return the number of entities currently managed by this system
    size_t GetEntityCount() const
    {
        return GetEntities().size();
    }
};

// ---------------------------------------------------------
// 1. Entity and Component Basic Lifecycle Test
// ---------------------------------------------------------
TEST(CoordinatorTest, EntityAndComponentLifecycle)
{
    auto coordinator = std::make_unique<Coordinator>();
    coordinator->Init();

    // Register components
    coordinator->RegisterComponent<TestHealthComponent>();
    coordinator->RegisterComponent<TestPositionComponent>();

    // Create an entity
    Entity entity = coordinator->CreateEntity();

    // Add component and verify
    coordinator->AddComponent(entity, TestHealthComponent{ 100 });
    EXPECT_TRUE(coordinator->HasComponent<TestHealthComponent>(entity));
    EXPECT_FALSE(coordinator->HasComponent<TestPositionComponent>(entity));

    // Retrieve and update component values
    auto& health = coordinator->GetComponent<TestHealthComponent>(entity);
    EXPECT_EQ(health.hp, 100);
    health.hp = 50;
    EXPECT_EQ(coordinator->GetComponent<TestHealthComponent>(entity).hp, 50);

    // Remove component
    coordinator->RemoveComponent<TestHealthComponent>(entity);
    EXPECT_FALSE(coordinator->HasComponent<TestHealthComponent>(entity));

    // Destroy entity (Ensure it exits gracefully without crashing)
    coordinator->DestroyEntity(entity);
    SUCCEED();
}

// ---------------------------------------------------------
// 2. System and Signature Integration Test
// ---------------------------------------------------------
TEST(CoordinatorTest, SystemSignatureIntegration)
{
    auto coordinator = std::make_unique<Coordinator>();
    coordinator->Init();

    coordinator->RegisterComponent<TestHealthComponent>();
    coordinator->RegisterComponent<TestPositionComponent>();

    // Configure the dummy system to require BOTH Health and Position components
    Signature testSignature;
    testSignature.set(coordinator->GetComponentType<TestHealthComponent>());
    testSignature.set(coordinator->GetComponentType<TestPositionComponent>());

    auto system = coordinator->RegisterSystem<TestHealthSystem>(
        testSignature, static_cast<SystemPhase>(0), 0
    );

    Entity e1 = coordinator->CreateEntity();
    Entity e2 = coordinator->CreateEntity();

    // 1. Add only Health to e1 -> Requirements not met, system entity count remains 0
    coordinator->AddComponent(e1, TestHealthComponent{ 100 });
    EXPECT_EQ(system->GetEntityCount(), 0);

    // 2. Add Position to e1 -> Requirements met, should be added to the system
    coordinator->AddComponent(e1, TestPositionComponent{ 0.0f, 0.0f });
    EXPECT_EQ(system->GetEntityCount(), 1);

    // 3. Add both to e2 -> System should now manage 2 entities
    coordinator->AddComponent(e2, TestHealthComponent{ 50 });
    coordinator->AddComponent(e2, TestPositionComponent{ 10.0f, 10.0f });
    EXPECT_EQ(system->GetEntityCount(), 2);

    // 4. Remove Position from e1 -> Signature mismatch, should be expelled from the system
    coordinator->RemoveComponent<TestPositionComponent>(e1);
    EXPECT_EQ(system->GetEntityCount(), 1);

    // 5. Destroy e2 -> Automatically wiped from the system's tracked entities
    coordinator->DestroyEntity(e2);
    EXPECT_EQ(system->GetEntityCount(), 0);
}

// ---------------------------------------------------------
// 3. AssignComponent Smart Insertion Test
// ---------------------------------------------------------
TEST(CoordinatorTest, AssignComponentAddsOrUpdatesAutomatically)
{
    auto coordinator = std::make_unique<Coordinator>();
    coordinator->Init();
    coordinator->RegisterComponent<TestHealthComponent>();

    Entity entity = coordinator->CreateEntity();

    // 1. Entity does not have the component, so AssignComponent should ADD it
    coordinator->AssignComponent(entity, TestHealthComponent{ 80 });
    ASSERT_TRUE(coordinator->HasComponent<TestHealthComponent>(entity));
    EXPECT_EQ(coordinator->GetComponent<TestHealthComponent>(entity).hp, 80);

    // 2. Entity already has it, so AssignComponent should SET (update) the existing one
    coordinator->AssignComponent(entity, TestHealthComponent{ 40 });
    EXPECT_EQ(coordinator->GetComponent<TestHealthComponent>(entity).hp, 40);
}

// ---------------------------------------------------------
// 4. Core Component Automatic Attachment Test
// ---------------------------------------------------------
TEST(CoordinatorTest, ApplyCoreComponentsTriggersCorrectly)
{
    auto coordinator = std::make_unique<Coordinator>();
    coordinator->Init();
    coordinator->RegisterComponent<TestCoreComponent>();

    Entity entity = coordinator->CreateEntity();

    // Explicitly apply core components (usually called internally during creation)
    coordinator->ApplyCoreComponents(entity);

    // Verify that the mandatory core component was automatically attached
    EXPECT_TRUE(coordinator->HasComponent<TestCoreComponent>(entity));
}

// ---------------------------------------------------------
// 5. Deferred Commands and Thread Buffer Flushing Test
// ---------------------------------------------------------
TEST(CoordinatorTest, DeferredCommandsAreAppliedAfterFlush)
{
    auto coordinator = std::make_unique<Coordinator>();
    coordinator->Init();
    coordinator->RegisterComponent<TestHealthComponent>();

    Entity entity = coordinator->CreateEntity();

    // Defer the component assignment (simulating a worker thread request)
    coordinator->DeferAssignComponent(entity, TestHealthComponent{ 999 });

    // Ensure it hasn't been applied yet
    EXPECT_FALSE(coordinator->HasComponent<TestHealthComponent>(entity));

    // Simulate the completion of a system phase.
    // The Coordinator should flush all thread command buffers at the end of the phase.
    coordinator->UpdatePhase(static_cast<SystemPhase>(0), 0.0f, 0.0);

    // Now the deferred operation should be strictly resolved and applied
    ASSERT_TRUE(coordinator->HasComponent<TestHealthComponent>(entity));
    EXPECT_EQ(coordinator->GetComponent<TestHealthComponent>(entity).hp, 999);
}
