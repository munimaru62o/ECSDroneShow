#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Game/Systems/FormationSystemWireframe.h"
#include "Game/Managers/TimelineManager.h"
#include "Game/Data/Timeline/FormationData.h"

class FormationWireframeTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    FormationSystemWireframe* system = nullptr;

    // sonar-ignore-next-line
    TimelineManager timelineManager;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<FormationSystemWireframe>();
        system->SetTimelineManager(&timelineManager);
    }

    /**
     * @brief Registers a simple line formation (2 points) into the TimelineManager cache.
     * Line from {0, 0, 0} to {100, 0, 0}.
     */
    void RegisterLineFormation(const std::string& key)
    {
        FormationData data;
        data.name = key;
        data.points.push_back({ Vector3{ 0.0f, 0.0f, 0.0f }, Color::White() });
        data.points.push_back({ Vector3{ 100.0f, 0.0f, 0.0f }, Color::White() });
        timelineManager.RegisterFormationData(key, std::move(data));
    }

    /**
     * @brief Registers a triangle formation (3 points) into the TimelineManager cache.
     * Triangle: {0,0,0} -> {100,0,0} -> {50,100,0}
     */
    void RegisterTriangleFormation(const std::string& key)
    {
        FormationData data;
        data.name = key;
        data.points.push_back({ Vector3{ 0.0f, 0.0f, 0.0f }, Color::White() });
        data.points.push_back({ Vector3{ 100.0f, 0.0f, 0.0f }, Color::White() });
        data.points.push_back({ Vector3{ 50.0f, 100.0f, 0.0f }, Color::White() });
        timelineManager.RegisterFormationData(key, std::move(data));
    }

    /**
     * @brief Registers a single-point formation.
     */
    void RegisterSinglePointFormation(const std::string& key)
    {
        FormationData data;
        data.name = key;
        data.points.push_back({ Vector3{ 42.0f, 0.0f, 0.0f }, Color::White() });
        timelineManager.RegisterFormationData(key, std::move(data));
    }
};

// ---------------------------------------------------------
// 1. Line Distribution: Verify 2 drones are placed at the endpoints of a 2-point formation
// ---------------------------------------------------------
TEST_F(FormationWireframeTestFixture, Wireframe_DistributesTwoDronesAlongLine)
{
    const std::string formationKey = "test_line";
    RegisterLineFormation(formationKey);

    // Arrange: 2 entities for a 2-point line (1 edge)
    Entity e0 = coordinator.CreateEntity();
    Entity e1 = coordinator.CreateEntity();

    FormationComponentWireframe formation{ .scale = 1.0f, .file = formationKey };

    coordinator.AddComponent(e0, DroneComponent{});
    coordinator.AddComponent(e0, TargetComponent{});
    coordinator.AddComponent(e0, DirectionComponent{});
    coordinator.AddComponent(e0, formation);

    coordinator.AddComponent(e1, DroneComponent{});
    coordinator.AddComponent(e1, TargetComponent{});
    coordinator.AddComponent(e1, DirectionComponent{});
    coordinator.AddComponent(e1, formation);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: First entity at start {0,0,0}, last entity snapped to end {100,0,0}
    auto& target0 = coordinator.GetComponent<TargetComponent>(e0).value;
    auto& target1 = coordinator.GetComponent<TargetComponent>(e1).value;

    EXPECT_NEAR(target0.x, 0.0f, 0.01f);
    EXPECT_NEAR(target1.x, 100.0f, 0.01f);
}

// ---------------------------------------------------------
// 2. Direction: Verify direction component aligns with the wireframe edge direction
// ---------------------------------------------------------
TEST_F(FormationWireframeTestFixture, Wireframe_DirectionFollowsEdge)
{
    const std::string formationKey = "test_line";
    RegisterLineFormation(formationKey);

    Entity e0 = coordinator.CreateEntity();
    FormationComponentWireframe formation{ .scale = 1.0f, .file = formationKey };

    coordinator.AddComponent(e0, DroneComponent{});
    coordinator.AddComponent(e0, TargetComponent{});
    coordinator.AddComponent(e0, DirectionComponent{});
    coordinator.AddComponent(e0, formation);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Edge direction from {0,0,0} to {100,0,0} is {1,0,0}
    auto& direction = coordinator.GetComponent<DirectionComponent>(e0).value;
    EXPECT_NEAR(direction.x, 1.0f, 0.001f);
    EXPECT_NEAR(direction.y, 0.0f, 0.001f);
    EXPECT_NEAR(direction.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Scaling: Verify scale parameter is applied to target positions
// ---------------------------------------------------------
TEST_F(FormationWireframeTestFixture, Wireframe_AppliesScale)
{
    const std::string formationKey = "test_line";
    RegisterLineFormation(formationKey);

    // Arrange: scale = 2.0, so endpoints become {0,0,0} and {200,0,0}
    Entity e0 = coordinator.CreateEntity();
    Entity e1 = coordinator.CreateEntity();

    FormationComponentWireframe formation{ .scale = 2.0f, .file = formationKey };

    coordinator.AddComponent(e0, DroneComponent{});
    coordinator.AddComponent(e0, TargetComponent{});
    coordinator.AddComponent(e0, DirectionComponent{});
    coordinator.AddComponent(e0, formation);

    coordinator.AddComponent(e1, DroneComponent{});
    coordinator.AddComponent(e1, TargetComponent{});
    coordinator.AddComponent(e1, DirectionComponent{});
    coordinator.AddComponent(e1, formation);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Last entity should be at {100, 0, 0} * 2.0 = {200, 0, 0}
    auto& target1 = coordinator.GetComponent<TargetComponent>(e1).value;
    EXPECT_NEAR(target1.x, 200.0f, 0.01f);
}

// ---------------------------------------------------------
// 4. Single Point: Verify all drones collapse to the single vertex
// ---------------------------------------------------------
TEST_F(FormationWireframeTestFixture, Wireframe_SinglePointCollapsesAll)
{
    const std::string formationKey = "test_single";
    RegisterSinglePointFormation(formationKey);

    Entity e0 = coordinator.CreateEntity();
    Entity e1 = coordinator.CreateEntity();

    FormationComponentWireframe formation{ .scale = 1.0f, .file = formationKey };

    coordinator.AddComponent(e0, DroneComponent{});
    coordinator.AddComponent(e0, TargetComponent{});
    coordinator.AddComponent(e0, DirectionComponent{});
    coordinator.AddComponent(e0, formation);

    coordinator.AddComponent(e1, DroneComponent{});
    coordinator.AddComponent(e1, TargetComponent{});
    coordinator.AddComponent(e1, DirectionComponent{});
    coordinator.AddComponent(e1, formation);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Both entities collapse to the single point {42, 0, 0}
    auto& target0 = coordinator.GetComponent<TargetComponent>(e0).value;
    auto& target1 = coordinator.GetComponent<TargetComponent>(e1).value;
    EXPECT_NEAR(target0.x, 42.0f, 0.01f);
    EXPECT_NEAR(target1.x, 42.0f, 0.01f);
}

// ---------------------------------------------------------
// 5. Multi-Edge Distribution: Verify even distribution across multiple edges
// ---------------------------------------------------------
TEST_F(FormationWireframeTestFixture, Wireframe_DistributesAcrossMultipleEdges)
{
    const std::string formationKey = "test_triangle";
    RegisterTriangleFormation(formationKey);

    // Arrange: 4 entities for a 3-point formation (2 edges)
    // Edge 0: {0,0,0} -> {100,0,0}
    // Edge 1: {100,0,0} -> {50,100,0}
    // baseCount = 4/2 = 2, remainder = 0. So each edge gets 2 entities.
    std::vector<Entity> entities;
    FormationComponentWireframe formation{ .scale = 1.0f, .file = formationKey };

    for (int i = 0; i < 4; ++i) {
        Entity e = coordinator.CreateEntity();
        coordinator.AddComponent(e, DroneComponent{});
        coordinator.AddComponent(e, TargetComponent{});
        coordinator.AddComponent(e, DirectionComponent{});
        coordinator.AddComponent(e, formation);
        entities.push_back(e);
    }

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: First entity on Edge 0 at t=0 -> {0, 0, 0}
    auto& target0 = coordinator.GetComponent<TargetComponent>(entities[0]).value;
    EXPECT_NEAR(target0.x, 0.0f, 0.01f);
    EXPECT_NEAR(target0.y, 0.0f, 0.01f);

    // Assert: Last entity snapped to terminal vertex {50, 100, 0}
    auto& targetLast = coordinator.GetComponent<TargetComponent>(entities[3]).value;
    EXPECT_NEAR(targetLast.x, 50.0f, 0.01f);
    EXPECT_NEAR(targetLast.y, 100.0f, 0.01f);
}
