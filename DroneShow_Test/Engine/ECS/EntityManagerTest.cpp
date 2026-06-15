#include "pch.h"
#include "Engine/ECS/EntityManager.h"

// ---------------------------------------------------------
// 1. Entity Creation and Destruction ID Issuance Test
// ---------------------------------------------------------
TEST(EntityManagerTest, CreateAndDestroyEntity)
{
    auto em = std::make_unique<EntityManager>();

    // 1. Initially, IDs should be issued in strict sequential order: 0, 1, 2...
    Entity e1 = em->CreateEntity();
    Entity e2 = em->CreateEntity();
    EXPECT_EQ(e1, 0);
    EXPECT_EQ(e2, 1);

    // 2. Destroy e1 (ID: 0)
    em->DestroyEntity(e1);

    // 3. The next created Entity should be 2, not 0!
    // Reclaimed IDs are pushed to the back of the queue to prevent immediate reuse,
    // mitigating bugs caused by dangling Entity ID references.
    Entity e3 = em->CreateEntity();
    EXPECT_EQ(e3, 2);
}

// ---------------------------------------------------------
// 2. Signature Management Test
// ---------------------------------------------------------
TEST(EntityManagerTest, SignatureManagement)
{
    auto em = std::make_unique<EntityManager>();
    Entity e = em->CreateEntity();

    Signature sig;
    sig.set(1, true);
    sig.set(5, true);

    // Verify that the signature is correctly applied
    em->SetSignature(e, sig);
    EXPECT_EQ(em->GetSignature(e), sig);

    // Verify that destroying the Entity completely resets its signature
    em->DestroyEntity(e);
    EXPECT_EQ(em->GetSignature(e).count(), 0); // All bits should be cleared
}

// ---------------------------------------------------------
// 3. Living Entity Count Tracking Test
// ---------------------------------------------------------
TEST(EntityManagerTest, LivingEntityCountTracker)
{
    auto em = std::make_unique<EntityManager>();

    // Initially, there should be 0 living entities
    EXPECT_EQ(em->GetLivingEntityCount(), 0);

    Entity e1 = em->CreateEntity();
    Entity e2 = em->CreateEntity();
    Entity e3 = em->CreateEntity();

    // After creating 3 entities, the count should be 3
    EXPECT_EQ(em->GetLivingEntityCount(), 3);

    // Destroy one entity, the count should drop to 2
    em->DestroyEntity(e2);
    EXPECT_EQ(em->GetLivingEntityCount(), 2);

    // Create another entity, the count should increase back to 3
    Entity e4 = em->CreateEntity();
    EXPECT_EQ(em->GetLivingEntityCount(), 3);
}
