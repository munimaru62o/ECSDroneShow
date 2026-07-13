#include "pch.h"
#include "Engine/Scene/Camera.h"
#include "Engine/Math/Vector2.h"

TEST(CameraTest, SettersAndGetters)
{
    Camera camera;
    camera.SetViewport(800, 600);
    
    Vector3 pos{ 10.0f, 20.0f, 30.0f };
    camera.SetPosition(pos);
    
    EXPECT_FLOAT_EQ(camera.GetPosition().x, pos.x);
    EXPECT_FLOAT_EQ(camera.GetPosition().y, pos.y);
    EXPECT_FLOAT_EQ(camera.GetPosition().z, pos.z);
}

TEST(CameraTest, LookAt)
{
    Camera camera;
    camera.SetViewport(800, 600);
    
    Vector3 eye{ 0.0f, 0.0f, 0.0f };
    Vector3 target{ 0.0f, 0.0f, 1.0f };
    camera.LookAt(eye, target, Vector3::Up());
    
    EXPECT_FLOAT_EQ(camera.GetPosition().x, eye.x);
    EXPECT_FLOAT_EQ(camera.GetPosition().y, eye.y);
    EXPECT_FLOAT_EQ(camera.GetPosition().z, eye.z);
    
    // Z forward rotation
    Vector3 forward = camera.GetRotation().Forward();
    EXPECT_NEAR(forward.x, 0.0f, 0.001f);
    EXPECT_NEAR(forward.y, 0.0f, 0.001f);
    EXPECT_NEAR(forward.z, 1.0f, 0.001f);
}

TEST(CameraTest, MatricesUpdate)
{
    Camera camera;
    camera.SetViewport(800, 600);
    camera.SetPerspective(45.0f, 0.1f, 1000.0f);
    camera.LookAt(Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 1.0f});

    const Matrix4& view = camera.GetViewMatrix();
    const Matrix4& proj = camera.GetProjectionMatrix();
    const Matrix4& vp = camera.GetViewProjectionMatrix();
    
    // View should be essentially identity except looking at +Z might flip based on handedness.
    // Projection should have valid values.
    EXPECT_GT(proj.m[0][0], 0.0f);
    EXPECT_GT(proj.m[1][1], 0.0f);
}

TEST(CameraTest, WorldToScreen)
{
    Camera camera;
    camera.SetViewport(800, 600);
    camera.SetPerspective(90.0f, 0.1f, 100.0f);
    camera.LookAt(Vector3{0.0f, 0.0f, -5.0f}, Vector3{0.0f, 0.0f, 0.0f}); // Looking at origin from -5 Z

    Vector2 screenPos;
    // World origin should be in the center of the screen
    bool result = camera.WorldToScreen(Vector3{0.0f, 0.0f, 0.0f}, screenPos);
    
    EXPECT_TRUE(result);
    EXPECT_NEAR(screenPos.x, 400.0f, 1.0f);
    EXPECT_NEAR(screenPos.y, 300.0f, 1.0f);
}
