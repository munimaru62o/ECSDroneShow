#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Common/TempDirectory.h"
#include "Game/Managers/PrefabManager.h"
#include "Game/Components/GameComponents.h"
#include "Engine/Components/PhysicsComponents.h"

class PrefabManagerTest : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    TempDirectory temp;
};

// ---------------------------------------------------------
// 1. Unknown Prefab: Instantiate returns INVALID_ENTITY and creates nothing
// ---------------------------------------------------------
TEST_F(PrefabManagerTest, InstantiateUnknownPrefabReturnsInvalidEntity)
{
    PrefabManager prefabManager(coordinator, registry);
    const uint32_t livingBefore = coordinator.GetLivingEntityCount();

    EXPECT_EQ(prefabManager.Instantiate("NotRegistered"), INVALID_ENTITY);
    EXPECT_EQ(coordinator.GetLivingEntityCount(), livingBefore);
}

// ---------------------------------------------------------
// 2. Directory Load: Only valid .json prefabs are loaded and can be instantiated
// ---------------------------------------------------------
TEST_F(PrefabManagerTest, LoadsPrefabsFromDirectoryAndInstantiates)
{
    temp.WriteFile("drone.json", R"({
        "name": "TestDrone",
        "components": {
            "DroneComponent": {},
            "DampingComponent": { "factor": 0.5 }
        }
    })");
    temp.WriteFile("broken.json", R"({"name": )");
    temp.WriteFile("readme.txt", "not a prefab");

    PrefabManager prefabManager(coordinator, registry);
    EXPECT_EQ(prefabManager.LoadPrefabsFromDirectory(temp.RootPath()), 1);

    Entity entity = prefabManager.Instantiate("TestDrone");
    ASSERT_NE(entity, INVALID_ENTITY);
    EXPECT_TRUE(coordinator.HasComponent<DroneComponent>(entity));
    ASSERT_TRUE(coordinator.HasComponent<DampingComponent>(entity));
    EXPECT_FLOAT_EQ(coordinator.GetComponent<DampingComponent>(entity).factor, 0.5f);
}
