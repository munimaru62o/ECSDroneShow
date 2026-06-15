#include "pch.h"
#include "Engine/ECS/EntitySparseSet.h"

// ---------------------------------------------------------
// 1. Basic Addition and Removal Test
// ---------------------------------------------------------
TEST(EntitySparseSetTest, AddAndRemoveEntity)
{
    EntitySparseSet sparseSet;

    // 1. Verify initial state
    EXPECT_EQ(sparseSet.Size(), 0);
    EXPECT_FALSE(sparseSet.Has(5));

    // 2. Test adding entities
    sparseSet.Add(5);
    sparseSet.Add(10);

    EXPECT_EQ(sparseSet.Size(), 2);
    EXPECT_TRUE(sparseSet.Has(5));
    EXPECT_TRUE(sparseSet.Has(10));

    // 3. Test O(1) removal (Swap and Pop)
    sparseSet.Remove(5);

    EXPECT_EQ(sparseSet.Size(), 1);
    EXPECT_FALSE(sparseSet.Has(5));  // 5 should be removed
    EXPECT_TRUE(sparseSet.Has(10));  // 10 should safely remain
}

// ---------------------------------------------------------
// 2. Duplicate Add Idempotency Test
//    Ensure that adding the same Entity twice does not corrupt the size or state.
// ---------------------------------------------------------
TEST(EntitySparseSetTest, DuplicateAddIsIdempotent)
{
    EntitySparseSet sparseSet;

    sparseSet.Add(3);
    sparseSet.Add(3); // The second addition should be safely ignored

    // Verify that the size remains 1
    EXPECT_EQ(sparseSet.Size(), 1);
    EXPECT_TRUE(sparseSet.Has(3));
}

// ---------------------------------------------------------
// 3. Safety Test for Removing Non-Existent Entities
//    Since Has() is checked internally, it should bypass the operation without crashing.
// ---------------------------------------------------------
TEST(EntitySparseSetTest, RemoveNonExistentEntityIsSafe)
{
    EntitySparseSet sparseSet;

    sparseSet.Add(7);

    // Attempting to remove a non-existent Entity (99) should be safely ignored
    sparseSet.Remove(99); // Verify it does not crash

    // The originally existing entity 7 must remain unaffected
    EXPECT_EQ(sparseSet.Size(), 1);
    EXPECT_TRUE(sparseSet.Has(7));
}

// ---------------------------------------------------------
// 4. GetIndex Validity Test
//    Ensure that surviving Entities retain correct dense array indices after a Swap and Pop.
// ---------------------------------------------------------
TEST(EntitySparseSetTest, GetIndexRemainsValidAfterRemove)
{
    EntitySparseSet sparseSet;

    // Added in order: 0 -> 1 -> 2 (Dense array indices will be 0, 1, 2)
    sparseSet.Add(0);
    sparseSet.Add(1);
    sparseSet.Add(2);

    // Remove Entity 1 (the middle element)
    // Internal behavior: Dense[2] (=Entity 2) is swapped into Dense[1], and Dense.pop_back() is called.
    sparseSet.Remove(1);

    // Verify that Entity 0 and Entity 2 remain
    EXPECT_TRUE(sparseSet.Has(0));
    EXPECT_FALSE(sparseSet.Has(1));
    EXPECT_TRUE(sparseSet.Has(2));

    // Verify that the indices returned by GetIndex() are within the valid bounds of the Dense array
    size_t idx0 = sparseSet.GetIndex(0);
    size_t idx2 = sparseSet.GetIndex(2);
    EXPECT_LT(idx0, sparseSet.Size());
    EXPECT_LT(idx2, sparseSet.Size());

    // The indices of the two remaining Entities must not overlap (they must point to distinct slots)
    EXPECT_NE(idx0, idx2);
}

// ---------------------------------------------------------
// 5. GetEntities Consistency Test
//    Ensure the dense array post-removal strictly matches the results of Has().
// ---------------------------------------------------------
TEST(EntitySparseSetTest, GetEntitiesConsistentWithHas)
{
    EntitySparseSet sparseSet;

    sparseSet.Add(10);
    sparseSet.Add(20);
    sparseSet.Add(30);

    sparseSet.Remove(20);

    // The array returned by GetEntities() must only contain the surviving Entities
    const auto& entities = sparseSet.GetEntities();
    EXPECT_EQ(entities.size(), 2);

    for (Entity e : entities) {
        // Every Entity present in the Dense array must return true for Has()
        EXPECT_TRUE(sparseSet.Has(e));
    }

    // The removed Entity must not exist in the Dense array
    bool found20 = false;
    for (Entity e : entities) {
        if (e == 20) {
            found20 = true;
        }
    }
    EXPECT_FALSE(found20);
}

// ---------------------------------------------------------
// 6. Clear State Reset Test
//    Ensure that calling Clear completely resets both Dense and Sparse arrays.
// ---------------------------------------------------------
TEST(EntitySparseSetTest, ClearResetsState)
{
    EntitySparseSet sparseSet;

    sparseSet.Add(100);
    sparseSet.Add(200);

    EXPECT_EQ(sparseSet.Size(), 2);

    // Execute Clear
    sparseSet.Clear();

    // Verify the size is 0 and previous entities no longer exist
    EXPECT_EQ(sparseSet.Size(), 0);
    EXPECT_FALSE(sparseSet.Has(100));
    EXPECT_FALSE(sparseSet.Has(200));

    // Verify that new additions work correctly after a Clear
    sparseSet.Add(300);
    EXPECT_EQ(sparseSet.Size(), 1);
    EXPECT_TRUE(sparseSet.Has(300));
}
