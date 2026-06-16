#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Animation/ColorFadeSystem.h"

class ColorFadeTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    ColorFadeSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<ColorFadeSystem>();
    }
};

// ---------------------------------------------------------
// 1. Mid-Fade: Verify color is interpolated at 50% through the duration
// ---------------------------------------------------------
TEST_F(ColorFadeTestFixture, Fade_InterpolatesAtMidpoint)
{
    // Arrange: Fade from Red to Blue over 2.0 seconds, created at time 0
    ColorFadeComponent fade;
    fade.startColor = Color::Red();
    fade.targetColor = Color::Blue();
    fade.duration = 2.0f;
    fade.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, MaterialComponent{ .color = Color::White() });
    coordinator.AddComponent(drone, fade);

    // Act: At simulationTime = 1.0, elapsed = 1.0. t = 1.0 / 2.0 = 0.5
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: Color should be a 50% blend of Red and Blue
    auto& color = coordinator.GetComponent<MaterialComponent>(drone).color;
    EXPECT_NEAR(color.r, 0.5f, 0.01f);
    EXPECT_NEAR(color.g, 0.0f, 0.01f);
    EXPECT_NEAR(color.b, 0.5f, 0.01f);
}

// ---------------------------------------------------------
// 2. Completion: Verify target color is applied and component is removed after duration
// ---------------------------------------------------------
TEST_F(ColorFadeTestFixture, Fade_CompletesToTargetColor)
{
    // Arrange: Fade from Red to Blue over 1.0 second, created at time 0
    ColorFadeComponent fade;
    fade.startColor = Color::Red();
    fade.targetColor = Color::Blue();
    fade.duration = 1.0f;
    fade.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, MaterialComponent{ .color = Color::White() });
    coordinator.AddComponent(drone, fade);

    // Act: At simulationTime = 2.0, elapsed = 2.0 > duration 1.0
    system->Update(coordinator, 0.016f, 2.0);

    // Process deferred removals
    coordinator.EndFrame();

    // Assert: Color should be exactly the target (Blue)
    auto& color = coordinator.GetComponent<MaterialComponent>(drone).color;
    EXPECT_NEAR(color.r, 0.0f, 0.001f);
    EXPECT_NEAR(color.g, 0.0f, 0.001f);
    EXPECT_NEAR(color.b, 1.0f, 0.001f);

    // Assert: ColorFadeComponent should be removed
    EXPECT_FALSE(coordinator.HasComponent<ColorFadeComponent>(drone));
}

// ---------------------------------------------------------
// 3. Early Fade: Verify color is near start color at the beginning
// ---------------------------------------------------------
TEST_F(ColorFadeTestFixture, Fade_StartsAtStartColor)
{
    // Arrange: Fade from White to Black over 10 seconds
    ColorFadeComponent fade;
    fade.startColor = Color::White();
    fade.targetColor = Color::Black();
    fade.duration = 10.0f;
    fade.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, MaterialComponent{ .color = Color::White() });
    coordinator.AddComponent(drone, fade);

    // Act: At simulationTime = 0.1, elapsed = 0.1. t = 0.1 / 10.0 = 0.01
    system->Update(coordinator, 0.016f, 0.1);

    // Assert: Color should be very close to White
    auto& color = coordinator.GetComponent<MaterialComponent>(drone).color;
    EXPECT_NEAR(color.r, 0.99f, 0.02f);
    EXPECT_NEAR(color.g, 0.99f, 0.02f);
    EXPECT_NEAR(color.b, 0.99f, 0.02f);
}
