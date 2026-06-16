#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Animation/ColorBlinkSystem.h"

class ColorBlinkTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    ColorBlinkSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<ColorBlinkSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Blink: Verify color interpolates between two colors
// ---------------------------------------------------------
TEST_F(ColorBlinkTestFixture, Blink_InterpolatesColors)
{
    // Arrange: Blink between Red and Blue, speed 1.0, no delay
    ColorBlinkComponent blink;
    blink.color1 = Color::Red();
    blink.color2 = Color::Blue();
    blink.speed = 1.0f;
    blink.delayTime = 0.0f;
    blink.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, MaterialComponent{ .color = Color::White() });
    coordinator.AddComponent(drone, blink);

    // Act: At simulationTime = 0.5, elapsed = 0.5. cycle = fmod(0.5 * 1.0, 2.0) = 0.5
    // t = 1.0 - abs(0.5 - 1.0) = 1.0 - 0.5 = 0.5
    system->Update(coordinator, 0.016f, 0.5);

    // Assert: Color should be a 50% blend of Red and Blue
    auto& color = coordinator.GetComponent<MaterialComponent>(drone).color;
    EXPECT_NEAR(color.r, 0.5f, 0.01f);
    EXPECT_NEAR(color.g, 0.0f, 0.01f);
    EXPECT_NEAR(color.b, 0.5f, 0.01f);
}

// ---------------------------------------------------------
// 2. Peak Color1: Verify color is exactly color1 at t=0
// ---------------------------------------------------------
TEST_F(ColorBlinkTestFixture, Blink_StartsAtColor1)
{
    // Arrange
    ColorBlinkComponent blink;
    blink.color1 = Color::Red();
    blink.color2 = Color::Blue();
    blink.speed = 1.0f;
    blink.delayTime = 0.0f;
    blink.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, MaterialComponent{ .color = Color::White() });
    coordinator.AddComponent(drone, blink);

    // Act: At simulationTime near 0 (tiny epsilon to pass the delay check)
    // elapsed = 0.001. cycle = fmod(0.001, 2.0) = 0.001. t = 1 - abs(0.001 - 1) = 0.001
    system->Update(coordinator, 0.016f, 0.001);

    // Assert: Color should be very close to color1 (Red)
    auto& color = coordinator.GetComponent<MaterialComponent>(drone).color;
    EXPECT_NEAR(color.r, 1.0f, 0.02f);
    EXPECT_NEAR(color.b, 0.0f, 0.02f);
}

// ---------------------------------------------------------
// 3. Delay: Verify no color change during delay period
// ---------------------------------------------------------
TEST_F(ColorBlinkTestFixture, Blink_RespectsDelay)
{
    // Arrange: Delay of 5.0 seconds
    ColorBlinkComponent blink;
    blink.color1 = Color::Red();
    blink.color2 = Color::Blue();
    blink.speed = 1.0f;
    blink.delayTime = 5.0f;
    blink.creationTime = 0.0;

    Color initialColor = Color::White();
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, MaterialComponent{ .color = initialColor });
    coordinator.AddComponent(drone, blink);

    // Act: simulationTime = 2.0, elapsed = 2.0 - 5.0 = -3.0 (still in delay)
    system->Update(coordinator, 0.016f, 2.0);

    // Assert: Color should remain unchanged (White)
    auto& color = coordinator.GetComponent<MaterialComponent>(drone).color;
    EXPECT_NEAR(color.r, initialColor.r, 0.001f);
    EXPECT_NEAR(color.g, initialColor.g, 0.001f);
    EXPECT_NEAR(color.b, initialColor.b, 0.001f);
}
