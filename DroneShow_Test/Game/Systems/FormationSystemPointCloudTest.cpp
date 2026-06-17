#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Game/Systems/FormationSystemPointCloud.h"
#include "Game/Managers/TimelineManager.h"
#include "Game/Data/Timeline/FormationData.h"

class FormationPointCloudTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    FormationSystemPointCloud* system = nullptr;

    // sonar-ignore-next-line
    TimelineManager timelineManager;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<FormationSystemPointCloud>();
        system->SetTimelineManager(&timelineManager);
    }

    /**
     * @brief Registers a 3-point formation with distinct colors.
     */
    void RegisterThreePointFormation(const std::string& key)
    {
        FormationData data;
        data.name = key;
        data.points.push_back({ Vector3{ 0.0f, 0.0f, 0.0f }, Color::Red() });
        data.points.push_back({ Vector3{ 100.0f, 0.0f, 0.0f }, Color::Green() });
        data.points.push_back({ Vector3{ 200.0f, 0.0f, 0.0f }, Color::Blue() });
        timelineManager.RegisterFormationData(key, std::move(data));
    }

    /**
     * @brief Registers a single-point formation.
     */
    void RegisterSinglePointFormation(const std::string& key)
    {
        FormationData data;
        data.name = key;
        data.points.push_back({ Vector3{ 42.0f, 0.0f, 0.0f }, Color::Yellow() });
        timelineManager.RegisterFormationData(key, std::move(data));
    }
};

// ---------------------------------------------------------
// 1. Exact Match: Verify 3 drones map 1:1 to 3 point cloud vertices
// ---------------------------------------------------------
TEST_F(FormationPointCloudTestFixture, PointCloud_ExactMapping)
{
    const std::string formationKey = "test_3pt";
    RegisterThreePointFormation(formationKey);

    // Arrange: 3 entities for 3 points (1:1 mapping)
    Entity e0 = coordinator.CreateEntity();
    Entity e1 = coordinator.CreateEntity();
    Entity e2 = coordinator.CreateEntity();

    FormationComponentPointCloud formation{ .scale = 1.0f, .file = formationKey };

    coordinator.AddComponent(e0, DroneComponent{});
    coordinator.AddComponent(e0, TargetComponent{});
    coordinator.AddComponent(e0, MaterialComponent{});
    coordinator.AddComponent(e0, formation);

    coordinator.AddComponent(e1, DroneComponent{});
    coordinator.AddComponent(e1, TargetComponent{});
    coordinator.AddComponent(e1, MaterialComponent{});
    coordinator.AddComponent(e1, formation);

    coordinator.AddComponent(e2, DroneComponent{});
    coordinator.AddComponent(e2, TargetComponent{});
    coordinator.AddComponent(e2, MaterialComponent{});
    coordinator.AddComponent(e2, formation);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Each entity maps to its corresponding point
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(e0).value.x, 0.0f, 0.01f);
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(e1).value.x, 100.0f, 0.01f);
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(e2).value.x, 200.0f, 0.01f);
}

// ---------------------------------------------------------
// 2. Color Assignment: Verify material color is set from point cloud data
// ---------------------------------------------------------
TEST_F(FormationPointCloudTestFixture, PointCloud_AssignsColor)
{
    const std::string formationKey = "test_3pt";
    RegisterThreePointFormation(formationKey);

    Entity e0 = coordinator.CreateEntity();
    FormationComponentPointCloud formation{ .scale = 1.0f, .file = formationKey };

    coordinator.AddComponent(e0, DroneComponent{});
    coordinator.AddComponent(e0, TargetComponent{});
    coordinator.AddComponent(e0, MaterialComponent{});
    coordinator.AddComponent(e0, formation);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: First point is Red
    auto& color = coordinator.GetComponent<MaterialComponent>(e0).color;
    EXPECT_NEAR(color.r, 1.0f, 0.001f);
    EXPECT_NEAR(color.g, 0.0f, 0.001f);
    EXPECT_NEAR(color.b, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Scaling: Verify scale parameter is applied to target positions
// ---------------------------------------------------------
TEST_F(FormationPointCloudTestFixture, PointCloud_AppliesScale)
{
    const std::string formationKey = "test_3pt";
    RegisterThreePointFormation(formationKey);

    Entity e0 = coordinator.CreateEntity();
    Entity e1 = coordinator.CreateEntity();

    FormationComponentPointCloud formation{ .scale = 3.0f, .file = formationKey };

    coordinator.AddComponent(e0, DroneComponent{});
    coordinator.AddComponent(e0, TargetComponent{});
    coordinator.AddComponent(e0, MaterialComponent{});
    coordinator.AddComponent(e0, formation);

    coordinator.AddComponent(e1, DroneComponent{});
    coordinator.AddComponent(e1, TargetComponent{});
    coordinator.AddComponent(e1, MaterialComponent{});
    coordinator.AddComponent(e1, formation);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Downsampled. e0 -> point[0] at {0,0,0}*3 = {0,0,0}
    // e1 -> point[1 or 2] based on downsampling step = 3/2 = 1.5
    // pointIndex for e1: int(1 * 1.5) = 1 -> {100,0,0}*3 = {300,0,0}
    auto& target0 = coordinator.GetComponent<TargetComponent>(e0).value;
    auto& target1 = coordinator.GetComponent<TargetComponent>(e1).value;

    EXPECT_NEAR(target0.x, 0.0f, 0.01f);
    EXPECT_NEAR(target1.x, 300.0f, 0.01f);
}

// ---------------------------------------------------------
// 4. More Drones Than Points (Modulo): Verify wrapping behavior
// ---------------------------------------------------------
TEST_F(FormationPointCloudTestFixture, PointCloud_ModuloDistribution)
{
    const std::string formationKey = "test_3pt";
    RegisterThreePointFormation(formationKey);

    // Arrange: 6 entities for 3 points -> modulo wrap (each point gets 2 entities)
    std::vector<Entity> entities;
    FormationComponentPointCloud formation{ .scale = 1.0f, .file = formationKey };

    for (int i = 0; i < 6; ++i) {
        Entity e = coordinator.CreateEntity();
        coordinator.AddComponent(e, DroneComponent{});
        coordinator.AddComponent(e, TargetComponent{});
        coordinator.AddComponent(e, MaterialComponent{});
        coordinator.AddComponent(e, formation);
        entities.push_back(e);
    }

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Entity 0 and 3 should map to point 0 (modulo)
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(entities[0]).value.x, 0.0f, 0.01f);
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(entities[3]).value.x, 0.0f, 0.01f);

    // Assert: Entity 1 and 4 should map to point 1
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(entities[1]).value.x, 100.0f, 0.01f);
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(entities[4]).value.x, 100.0f, 0.01f);

    // Assert: Entity 2 and 5 should map to point 2
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(entities[2]).value.x, 200.0f, 0.01f);
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(entities[5]).value.x, 200.0f, 0.01f);
}

// ---------------------------------------------------------
// 5. Fewer Drones Than Points (Downsampling): Verify uniform sampling
// ---------------------------------------------------------
TEST_F(FormationPointCloudTestFixture, PointCloud_DownsamplingPreservesShape)
{
    const std::string formationKey = "test_3pt";
    RegisterThreePointFormation(formationKey);

    // Arrange: 2 entities for 3 points -> downsampling
    Entity e0 = coordinator.CreateEntity();
    Entity e1 = coordinator.CreateEntity();

    FormationComponentPointCloud formation{ .scale = 1.0f, .file = formationKey };

    coordinator.AddComponent(e0, DroneComponent{});
    coordinator.AddComponent(e0, TargetComponent{});
    coordinator.AddComponent(e0, MaterialComponent{});
    coordinator.AddComponent(e0, formation);

    coordinator.AddComponent(e1, DroneComponent{});
    coordinator.AddComponent(e1, TargetComponent{});
    coordinator.AddComponent(e1, MaterialComponent{});
    coordinator.AddComponent(e1, formation);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Downsampling step = 3/2 = 1.5
    // e0: int(0 * 1.5) = 0 -> point[0] {0,0,0}
    // e1: int(1 * 1.5) = 1 -> point[1] {100,0,0}
    auto& target0 = coordinator.GetComponent<TargetComponent>(e0).value;
    auto& target1 = coordinator.GetComponent<TargetComponent>(e1).value;
    EXPECT_NEAR(target0.x, 0.0f, 0.01f);
    EXPECT_NEAR(target1.x, 100.0f, 0.01f);
}

// ---------------------------------------------------------
// 6. Single Point Collapse: Verify many drones map to a single point correctly
// ---------------------------------------------------------
TEST_F(FormationPointCloudTestFixture, PointCloud_SinglePointCollapse)
{
    const std::string formationKey = "test_1pt";
    RegisterSinglePointFormation(formationKey);

    // Arrange: 3 entities for 1 point (Many-to-One mapping)
    Entity e0 = coordinator.CreateEntity();
    Entity e1 = coordinator.CreateEntity();
    Entity e2 = coordinator.CreateEntity();

    FormationComponentPointCloud formation{ .scale = 1.0f, .file = formationKey };

    // Set up components
    for (Entity e : {e0, e1, e2}) {
        coordinator.AddComponent(e, DroneComponent{});
        coordinator.AddComponent(e, TargetComponent{});
        coordinator.AddComponent(e, MaterialComponent{});
        coordinator.AddComponent(e, formation);
    }

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: All entities should map to the single point {42.0f, 0, 0} via modulo (i % 1 == 0)
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(e0).value.x, 42.0f, 0.01f);
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(e1).value.x, 42.0f, 0.01f);
    EXPECT_NEAR(coordinator.GetComponent<TargetComponent>(e2).value.x, 42.0f, 0.01f);
}

// ---------------------------------------------------------
// 7. Zero Entities: Verify graceful exit when no entities match the signature
// ---------------------------------------------------------
TEST_F(FormationPointCloudTestFixture, PointCloud_ZeroEntities)
{
    // Arrange: No entities created.

    // Act & Assert: System should return early and not crash on `entities[0]`
    EXPECT_NO_THROW({
        system->Update(coordinator, 0.016f, 0.0);
                    });
}
