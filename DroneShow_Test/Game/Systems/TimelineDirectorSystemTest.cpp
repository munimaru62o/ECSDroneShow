#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Game/Systems/TimelineDirectorSystem.h"
#include "Game/Managers/TimelineManager.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Data/Timeline/TimelineData.h"

#include <json.hpp>

class TimelineDirectorTestFixture : public ECSTestFixture
{
protected:
    TimelineDirectorSystem* directorSystem;
    TimelineData timeline;

    void SetUpSystems() override
    {
        directorSystem = coordinator.GetSystem<TimelineDirectorSystem>();
        directorSystem->SetRegistry(&registry);
    }

    // Constructs a mock timeline (JSON data) for testing
    void SetupTimeline()
    {
        // Sequence 0: Assigns LiftComponent(maxForce: 42), Duration 1.0 seconds
        SequenceData seq0;
        seq0.duration = 1.0f;
        seq0.components["LiftComponent"] = nlohmann::json::parse(R"({"maxForce": 42})");

        // Sequence 1: Assigns GravityComponent(strength: 3.14) (Lift is removed), Duration 2.0 seconds
        SequenceData seq1;
        seq1.duration = 2.0f;
        seq1.components["GravityComponent"] = nlohmann::json::parse(R"({"strength": 3.14})");

        timeline.sequences.push_back(seq0);
        timeline.sequences.push_back(seq1);

        directorSystem->SetTimeline(&timeline);
    }
};

// ---------------------------------------------------------
// 1. Initial State: Verifies if the first sequence's components are applied on the initial update
// ---------------------------------------------------------
TEST_F(TimelineDirectorTestFixture, Director_AppliesFirstSequenceComponents)
{
    SetupTimeline();

    // Arrange: Create a single drone entity
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, DroneComponent{});

    // Act: Initial update (Timer is <= 0, so it should immediately transition to Sequence 0)
    directorSystem->Update(coordinator, 0.016f, 0.0);

    // Assert: Verify LiftComponent is attached and values match the JSON parsing
    EXPECT_TRUE(coordinator.HasComponent<LiftComponent>(drone));
    EXPECT_FALSE(coordinator.HasComponent<GravityComponent>(drone)); // Sequence 1 component should not exist yet
    EXPECT_EQ(coordinator.GetComponent<LiftComponent>(drone).maxForce, 42);
}

// ---------------------------------------------------------
// 2. State Transition: Verifies transition to the next sequence and cleanup of obsolete components
// ---------------------------------------------------------
TEST_F(TimelineDirectorTestFixture, Director_RemovesOldComponentsOnTransition)
{
    SetupTimeline();

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, DroneComponent{});

    // Act 1: Initial update (Transitions to Sequence 0, timer becomes 1.0f)
    directorSystem->Update(coordinator, 0.0f, 0.0);

    // Act 2: Advance time beyond 1.0s to trigger the transition to Sequence 1
    directorSystem->Update(coordinator, 1.1f, 1.1);

    // Assert: LiftComponent must be removed, and GravityComponent must be freshly attached
    EXPECT_FALSE(coordinator.HasComponent<LiftComponent>(drone));    // Should be stripped off
    EXPECT_TRUE(coordinator.HasComponent<GravityComponent>(drone));  // Should be added
    EXPECT_FLOAT_EQ(coordinator.GetComponent<GravityComponent>(drone).strength, 3.14f);
}

// ---------------------------------------------------------
// 3. Catch-Up: Verifies state synchronization for entities joining mid-sequence
// ---------------------------------------------------------
TEST_F(TimelineDirectorTestFixture, Director_CatchUpAppliesCurrentSequence)
{
    SetupTimeline();

    // Existing drone
    Entity drone1 = coordinator.CreateEntity();
    coordinator.AddComponent(drone1, DroneComponent{});

    // Act 1: Initial update (Sequence 0)
    directorSystem->Update(coordinator, 0.0f, 0.0);
    // Act 2: Transition to Sequence 1
    directorSystem->Update(coordinator, 1.1f, 1.1);

    // Arrange: A new drone spawns mid-sequence (late joiner)
    Entity drone2 = coordinator.CreateEntity();
    coordinator.AddComponent(drone2, DroneComponent{});

    // Act 3: Execute CatchUp for the newly joined drone
    directorSystem->CatchUpNewEntity(coordinator, drone2);

    // Assert: The late-joining drone2 must have the exact state of Sequence 1 applied
    EXPECT_FALSE(coordinator.HasComponent<LiftComponent>(drone2));
    EXPECT_TRUE(coordinator.HasComponent<GravityComponent>(drone2));
    EXPECT_FLOAT_EQ(coordinator.GetComponent<GravityComponent>(drone2).strength, 3.14f);
}

// ---------------------------------------------------------
// 4. Looping: Verifies that the timeline successfully loops back to the start
// ---------------------------------------------------------
TEST_F(TimelineDirectorTestFixture, Director_LoopsBackToFirstSequence)
{
    SetupTimeline();

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, DroneComponent{});

    // Act 1: Init Sequence 0 (Duration: 1.0s)
    directorSystem->Update(coordinator, 0.0f, 0.0);

    // Act 2: Advance by 1.1s -> Transitions to Sequence 1 (Duration: 2.0s)
    directorSystem->Update(coordinator, 1.1f, 1.1);
    EXPECT_TRUE(coordinator.HasComponent<GravityComponent>(drone));

    // Act 3: Advance by another 2.1s -> Exceeds Sequence 1, must loop back to Sequence 0
    directorSystem->Update(coordinator, 2.1f, 3.2);

    // Assert: Timeline has looped. Gravity is stripped, Lift is restored.
    EXPECT_TRUE(coordinator.HasComponent<LiftComponent>(drone));
    EXPECT_FALSE(coordinator.HasComponent<GravityComponent>(drone));
}
