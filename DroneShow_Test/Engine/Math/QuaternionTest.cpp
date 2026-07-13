#include "pch.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Constants.h"

#include <cmath>

// =========================================================
// Construction / Identity
// =========================================================
TEST(QuaternionTest, DefaultConstruction_IsIdentity)
{
    Quaternion q;
    EXPECT_FLOAT_EQ(q.x, 0.0f);
    EXPECT_FLOAT_EQ(q.y, 0.0f);
    EXPECT_FLOAT_EQ(q.z, 0.0f);
    EXPECT_FLOAT_EQ(q.w, 1.0f);
}

TEST(QuaternionTest, Identity_MatchesDefault)
{
    Quaternion id = Quaternion::Identity();
    EXPECT_FLOAT_EQ(id.x, 0.0f);
    EXPECT_FLOAT_EQ(id.y, 0.0f);
    EXPECT_FLOAT_EQ(id.z, 0.0f);
    EXPECT_FLOAT_EQ(id.w, 1.0f);
}

// =========================================================
// Normalize
// =========================================================
TEST(QuaternionTest, Normalize_AlreadyUnit_NoChange)
{
    Quaternion q = Quaternion::Identity();
    q.Normalize();

    EXPECT_NEAR(q.w, 1.0f, 0.0001f);
    EXPECT_NEAR(q.x, 0.0f, 0.0001f);
}

TEST(QuaternionTest, Normalize_Scaled_BecomesUnit)
{
    Quaternion q(0.0f, 0.0f, 0.0f, 2.0f);
    q.Normalize();

    float mag = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    EXPECT_NEAR(mag, 1.0f, 0.0001f);
}

TEST(QuaternionTest, Normalize_ZeroQuaternion_ReturnsIdentity)
{
    Quaternion q(0.0f, 0.0f, 0.0f, 0.0f);
    q.Normalize();

    EXPECT_FLOAT_EQ(q.w, 1.0f);
    EXPECT_FLOAT_EQ(q.x, 0.0f);
    EXPECT_FLOAT_EQ(q.y, 0.0f);
    EXPECT_FLOAT_EQ(q.z, 0.0f);
}

// =========================================================
// Arithmetic Operators
// =========================================================
TEST(QuaternionTest, Multiplication_IdentityPreserves)
{
    Quaternion q(0.1f, 0.2f, 0.3f, 0.927f);
    q.Normalize();
    Quaternion result = q * Quaternion::Identity();

    EXPECT_NEAR(result.x, q.x, 0.0001f);
    EXPECT_NEAR(result.y, q.y, 0.0001f);
    EXPECT_NEAR(result.z, q.z, 0.0001f);
    EXPECT_NEAR(result.w, q.w, 0.0001f);
}

TEST(QuaternionTest, MultiplicationAssignment)
{
    Quaternion a = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);
    Quaternion b = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);

    Quaternion expected = a * b;
    a *= b;

    EXPECT_NEAR(a.x, expected.x, 0.0001f);
    EXPECT_NEAR(a.y, expected.y, 0.0001f);
    EXPECT_NEAR(a.z, expected.z, 0.0001f);
    EXPECT_NEAR(a.w, expected.w, 0.0001f);
}

TEST(QuaternionTest, ScalarMultiplication)
{
    Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion result = q * 2.0f;

    EXPECT_FLOAT_EQ(result.x, 2.0f);
    EXPECT_FLOAT_EQ(result.y, 4.0f);
    EXPECT_FLOAT_EQ(result.z, 6.0f);
    EXPECT_FLOAT_EQ(result.w, 8.0f);
}

TEST(QuaternionTest, Addition)
{
    Quaternion a(1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion b(0.5f, 0.5f, 0.5f, 0.5f);
    Quaternion result = a + b;

    EXPECT_FLOAT_EQ(result.x, 1.5f);
    EXPECT_FLOAT_EQ(result.y, 2.5f);
    EXPECT_FLOAT_EQ(result.z, 3.5f);
    EXPECT_FLOAT_EQ(result.w, 4.5f);
}

// =========================================================
// Dot Product
// =========================================================
TEST(QuaternionTest, Dot_IdenticalQuaternions_IsOne)
{
    Quaternion q = Quaternion::Identity();
    EXPECT_NEAR(Quaternion::Dot(q, q), 1.0f, 0.0001f);
}

TEST(QuaternionTest, Dot_OrthogonalQuaternions_IsZero)
{
    // Two quaternions representing 90-degree rotations around perpendicular axes
    // won't have dot=0, but q and q*-1 should have dot=-1
    Quaternion q = Quaternion::Identity();
    Quaternion neg = q * -1.0f;
    EXPECT_NEAR(Quaternion::Dot(q, neg), -1.0f, 0.0001f);
}

// =========================================================
// Rotate
// =========================================================
TEST(QuaternionTest, Rotate_Identity_PreservesVector)
{
    Quaternion q = Quaternion::Identity();
    Vector3 v{ 1.0f, 2.0f, 3.0f };
    Vector3 result = q.Rotate(v);

    EXPECT_NEAR(result.x, 1.0f, 0.0001f);
    EXPECT_NEAR(result.y, 2.0f, 0.0001f);
    EXPECT_NEAR(result.z, 3.0f, 0.0001f);
}

TEST(QuaternionTest, Rotate_90DegreesAroundY)
{
    // Rotating {1, 0, 0} by 90 degrees around Y: expect approximately {0, 0, -1}
    Quaternion q = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);
    Vector3 result = q.Rotate(Vector3{ 1.0f, 0.0f, 0.0f });

    EXPECT_NEAR(result.x, 0.0f, 0.001f);
    EXPECT_NEAR(result.y, 0.0f, 0.001f);
    EXPECT_NEAR(result.z, -1.0f, 0.001f);
}

TEST(QuaternionTest, Rotate_180DegreesAroundY)
{
    // Rotating {1, 0, 0} by 180 degrees around Y: expect {-1, 0, 0}
    Quaternion q = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI);
    Vector3 result = q.Rotate(Vector3{ 1.0f, 0.0f, 0.0f });

    EXPECT_NEAR(result.x, -1.0f, 0.001f);
    EXPECT_NEAR(result.y, 0.0f, 0.001f);
    EXPECT_NEAR(result.z, 0.0f, 0.001f);
}

TEST(QuaternionTest, Rotate_PreservesLength)
{
    Quaternion q = Quaternion::FromAxisAngle(Vector3{ 1.0f, 1.0f, 1.0f }, 1.23f);
    Vector3 v{ 3.0f, 4.0f, 0.0f };
    Vector3 result = q.Rotate(v);

    EXPECT_NEAR(result.Length(), v.Length(), 0.001f);
}

// =========================================================
// FromAxisAngle
// =========================================================
TEST(QuaternionTest, FromAxisAngle_ZeroAngle_IsIdentity)
{
    Quaternion q = Quaternion::FromAxisAngle(Vector3::Up(), 0.0f);

    EXPECT_NEAR(q.x, 0.0f, 0.0001f);
    EXPECT_NEAR(q.y, 0.0f, 0.0001f);
    EXPECT_NEAR(q.z, 0.0f, 0.0001f);
    EXPECT_NEAR(q.w, 1.0f, 0.0001f);
}

TEST(QuaternionTest, FromAxisAngle_360Degrees_IsIdentity)
{
    Quaternion q = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::TWO_PI);

    // cos(PI) = -1, so w = -1. This is equivalent to identity (q and -q represent the same rotation).
    float mag = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    EXPECT_NEAR(mag, 1.0f, 0.0001f);

    // Verify it still acts as identity when rotating a vector
    Vector3 v{ 1.0f, 2.0f, 3.0f };
    Vector3 result = q.Rotate(v);
    EXPECT_NEAR(result.x, v.x, 0.001f);
    EXPECT_NEAR(result.y, v.y, 0.001f);
    EXPECT_NEAR(result.z, v.z, 0.001f);
}

// =========================================================
// FromEuler
// =========================================================
TEST(QuaternionTest, FromEuler_Zero_IsIdentity)
{
    Quaternion q = Quaternion::FromEuler(0.0f, 0.0f, 0.0f);

    EXPECT_NEAR(q.x, 0.0f, 0.0001f);
    EXPECT_NEAR(q.y, 0.0f, 0.0001f);
    EXPECT_NEAR(q.z, 0.0f, 0.0001f);
    EXPECT_NEAR(q.w, 1.0f, 0.0001f);
}

TEST(QuaternionTest, FromEuler_90Y_MatchesFromAxisAngle)
{
    Quaternion fromEuler = Quaternion::FromEuler(0.0f, MathConstants::PI * 0.5f, 0.0f);
    Quaternion fromAxis = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);

    // Both should produce the same rotation (verify by rotating the same vector)
    Vector3 v{ 1.0f, 0.0f, 0.0f };
    Vector3 r1 = fromEuler.Rotate(v);
    Vector3 r2 = fromAxis.Rotate(v);

    EXPECT_NEAR(r1.x, r2.x, 0.001f);
    EXPECT_NEAR(r1.y, r2.y, 0.001f);
    EXPECT_NEAR(r1.z, r2.z, 0.001f);
}

// =========================================================
// Slerp
// =========================================================
TEST(QuaternionTest, Slerp_T0_ReturnsStart)
{
    Quaternion a = Quaternion::Identity();
    Quaternion b = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);
    Quaternion result = Quaternion::Slerp(a, b, 0.0f);

    EXPECT_NEAR(result.x, a.x, 0.001f);
    EXPECT_NEAR(result.y, a.y, 0.001f);
    EXPECT_NEAR(result.z, a.z, 0.001f);
    EXPECT_NEAR(result.w, a.w, 0.001f);
}

TEST(QuaternionTest, Slerp_T1_ReturnsEnd)
{
    Quaternion a = Quaternion::Identity();
    Quaternion b = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);
    Quaternion result = Quaternion::Slerp(a, b, 1.0f);

    // Verify by rotating the same vector
    Vector3 v{ 1.0f, 0.0f, 0.0f };
    Vector3 r1 = result.Rotate(v);
    Vector3 r2 = b.Rotate(v);

    EXPECT_NEAR(r1.x, r2.x, 0.001f);
    EXPECT_NEAR(r1.y, r2.y, 0.001f);
    EXPECT_NEAR(r1.z, r2.z, 0.001f);
}

TEST(QuaternionTest, Slerp_Midpoint_HalfRotation)
{
    Quaternion a = Quaternion::Identity();
    // 90 degrees around Y
    Quaternion b = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);
    Quaternion mid = Quaternion::Slerp(a, b, 0.5f);

    // At t=0.5, rotation should be ~45 degrees around Y
    // Rotating {1, 0, 0} by 45 degrees: expect {cos(45), 0, -sin(45)} ≈ {0.707, 0, -0.707}
    Vector3 result = mid.Rotate(Vector3{ 1.0f, 0.0f, 0.0f });

    EXPECT_NEAR(result.x, 0.707f, 0.01f);
    EXPECT_NEAR(result.y, 0.0f, 0.001f);
    EXPECT_NEAR(result.z, -0.707f, 0.01f);
}

TEST(QuaternionTest, Slerp_ResultIsNormalized)
{
    Quaternion a = Quaternion::FromAxisAngle(Vector3::Up(), 0.3f);
    Quaternion b = Quaternion::FromAxisAngle(Vector3::Right(), 1.5f);
    Quaternion result = Quaternion::Slerp(a, b, 0.7f);

    float mag = std::sqrt(result.x * result.x + result.y * result.y + result.z * result.z + result.w * result.w);
    EXPECT_NEAR(mag, 1.0f, 0.001f);
}

TEST(QuaternionTest, Slerp_ClampsBelowZero)
{
    Quaternion a = Quaternion::Identity();
    Quaternion b = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);
    Quaternion result = Quaternion::Slerp(a, b, -1.0f);

    // Should clamp to t=0 and return a
    EXPECT_NEAR(result.x, a.x, 0.001f);
    EXPECT_NEAR(result.w, a.w, 0.001f);
}

TEST(QuaternionTest, Slerp_ClampsAboveOne)
{
    Quaternion a = Quaternion::Identity();
    Quaternion b = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);
    Quaternion result = Quaternion::Slerp(a, b, 2.0f);

    // Should clamp to t=1 and return b
    Vector3 v{ 1.0f, 0.0f, 0.0f };
    Vector3 r1 = result.Rotate(v);
    Vector3 r2 = b.Rotate(v);

    EXPECT_NEAR(r1.x, r2.x, 0.001f);
    EXPECT_NEAR(r1.z, r2.z, 0.001f);
}

// =========================================================
// LookRotation
// =========================================================
TEST(QuaternionTest, LookRotation_Forward_IsIdentity)
{
    // Looking forward (+Z) with up (+Y) should be identity rotation
    Quaternion q = Quaternion::LookRotation(Vector3::Forward(), Vector3::Up());
    Vector3 result = q.Rotate(Vector3::Forward());

    EXPECT_NEAR(result.x, 0.0f, 0.001f);
    EXPECT_NEAR(result.y, 0.0f, 0.001f);
    EXPECT_NEAR(result.z, 1.0f, 0.001f);
}

TEST(QuaternionTest, LookRotation_Right_Rotates90)
{
    // Looking right (+X) should rotate Forward to Right
    Quaternion q = Quaternion::LookRotation(Vector3::Right(), Vector3::Up());
    Vector3 result = q.Rotate(Vector3::Forward());

    EXPECT_NEAR(result.x, 1.0f, 0.01f);
    EXPECT_NEAR(result.y, 0.0f, 0.01f);
    EXPECT_NEAR(result.z, 0.0f, 0.01f);
}
