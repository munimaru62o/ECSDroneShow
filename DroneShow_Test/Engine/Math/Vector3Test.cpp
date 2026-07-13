#include "pch.h"
#include "Engine/Math/Vector3.h"

// =========================================================
// Construction / Static Helpers
// =========================================================
TEST(Vector3Test, DefaultConstruction_IsZero)
{
    Vector3 v;
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
    EXPECT_FLOAT_EQ(v.z, 0.0f);
}

TEST(Vector3Test, StaticHelpers_ReturnCorrectDirections)
{
    EXPECT_FLOAT_EQ(Vector3::Up().y, 1.0f);
    EXPECT_FLOAT_EQ(Vector3::Down().y, -1.0f);
    EXPECT_FLOAT_EQ(Vector3::Forward().z, 1.0f);
    EXPECT_FLOAT_EQ(Vector3::Back().z, -1.0f);
    EXPECT_FLOAT_EQ(Vector3::Right().x, 1.0f);
    EXPECT_FLOAT_EQ(Vector3::Left().x, -1.0f);

    Vector3 zero = Vector3::Zero();
    EXPECT_FLOAT_EQ(zero.x, 0.0f);
    EXPECT_FLOAT_EQ(zero.y, 0.0f);
    EXPECT_FLOAT_EQ(zero.z, 0.0f);
}

// =========================================================
// Arithmetic Operators
// =========================================================
TEST(Vector3Test, Addition)
{
    Vector3 a{ 1.0f, 2.0f, 3.0f };
    Vector3 b{ 4.0f, 5.0f, 6.0f };
    Vector3 result = a + b;

    EXPECT_FLOAT_EQ(result.x, 5.0f);
    EXPECT_FLOAT_EQ(result.y, 7.0f);
    EXPECT_FLOAT_EQ(result.z, 9.0f);
}

TEST(Vector3Test, AdditionAssignment)
{
    Vector3 a{ 1.0f, 2.0f, 3.0f };
    a += Vector3{ 10.0f, 20.0f, 30.0f };

    EXPECT_FLOAT_EQ(a.x, 11.0f);
    EXPECT_FLOAT_EQ(a.y, 22.0f);
    EXPECT_FLOAT_EQ(a.z, 33.0f);
}

TEST(Vector3Test, Subtraction)
{
    Vector3 a{ 5.0f, 7.0f, 9.0f };
    Vector3 b{ 1.0f, 2.0f, 3.0f };
    Vector3 result = a - b;

    EXPECT_FLOAT_EQ(result.x, 4.0f);
    EXPECT_FLOAT_EQ(result.y, 5.0f);
    EXPECT_FLOAT_EQ(result.z, 6.0f);
}

TEST(Vector3Test, SubtractionAssignment)
{
    Vector3 a{ 10.0f, 20.0f, 30.0f };
    a -= Vector3{ 1.0f, 2.0f, 3.0f };

    EXPECT_FLOAT_EQ(a.x, 9.0f);
    EXPECT_FLOAT_EQ(a.y, 18.0f);
    EXPECT_FLOAT_EQ(a.z, 27.0f);
}

TEST(Vector3Test, UnaryNegation)
{
    Vector3 v{ 1.0f, -2.0f, 3.0f };
    Vector3 neg = -v;

    EXPECT_FLOAT_EQ(neg.x, -1.0f);
    EXPECT_FLOAT_EQ(neg.y, 2.0f);
    EXPECT_FLOAT_EQ(neg.z, -3.0f);
}

TEST(Vector3Test, ScalarMultiplication)
{
    Vector3 v{ 2.0f, 3.0f, 4.0f };
    Vector3 result = v * 3.0f;

    EXPECT_FLOAT_EQ(result.x, 6.0f);
    EXPECT_FLOAT_EQ(result.y, 9.0f);
    EXPECT_FLOAT_EQ(result.z, 12.0f);
}

TEST(Vector3Test, ScalarMultiplicationAssignment)
{
    Vector3 v{ 2.0f, 3.0f, 4.0f };
    v *= 0.5f;

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 1.5f);
    EXPECT_FLOAT_EQ(v.z, 2.0f);
}

TEST(Vector3Test, ScalarDivision)
{
    Vector3 v{ 10.0f, 20.0f, 30.0f };
    Vector3 result = v / 2.0f;

    EXPECT_FLOAT_EQ(result.x, 5.0f);
    EXPECT_FLOAT_EQ(result.y, 10.0f);
    EXPECT_FLOAT_EQ(result.z, 15.0f);
}

TEST(Vector3Test, ScalarDivisionAssignment)
{
    Vector3 v{ 12.0f, 6.0f, 3.0f };
    v /= 3.0f;

    EXPECT_FLOAT_EQ(v.x, 4.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
    EXPECT_FLOAT_EQ(v.z, 1.0f);
}

// =========================================================
// Length / Distance
// =========================================================
TEST(Vector3Test, LengthSq)
{
    Vector3 v{ 3.0f, 4.0f, 0.0f };
    EXPECT_FLOAT_EQ(v.LengthSq(), 25.0f);
}

TEST(Vector3Test, Length)
{
    Vector3 v{ 3.0f, 4.0f, 0.0f };
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);
}

TEST(Vector3Test, Length_ZeroVector)
{
    Vector3 v = Vector3::Zero();
    EXPECT_FLOAT_EQ(v.Length(), 0.0f);
}

TEST(Vector3Test, DistanceSq)
{
    Vector3 a{ 1.0f, 0.0f, 0.0f };
    Vector3 b{ 4.0f, 4.0f, 0.0f };
    EXPECT_FLOAT_EQ(Vector3::DistanceSq(a, b), 25.0f);
}

TEST(Vector3Test, Distance)
{
    Vector3 a{ 1.0f, 0.0f, 0.0f };
    Vector3 b{ 4.0f, 4.0f, 0.0f };
    EXPECT_FLOAT_EQ(Vector3::Distance(a, b), 5.0f);
}

TEST(Vector3Test, Distance_SamePoint_IsZero)
{
    Vector3 a{ 5.0f, 5.0f, 5.0f };
    EXPECT_FLOAT_EQ(Vector3::Distance(a, a), 0.0f);
}

// =========================================================
// Normalize
// =========================================================
TEST(Vector3Test, Normalize_UnitLength)
{
    Vector3 v{ 3.0f, 4.0f, 0.0f };
    v.Normalize();

    EXPECT_NEAR(v.Length(), 1.0f, 0.0001f);
    EXPECT_NEAR(v.x, 0.6f, 0.0001f);
    EXPECT_NEAR(v.y, 0.8f, 0.0001f);
}

TEST(Vector3Test, Normalize_ZeroVector_StaysZero)
{
    Vector3 v = Vector3::Zero();
    v.Normalize();

    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
    EXPECT_FLOAT_EQ(v.z, 0.0f);
}

TEST(Vector3Test, Normalized_DoesNotModifyOriginal)
{
    Vector3 original{ 3.0f, 4.0f, 0.0f };
    Vector3 norm = original.Normalized();

    // Original is unchanged
    EXPECT_FLOAT_EQ(original.x, 3.0f);
    EXPECT_FLOAT_EQ(original.y, 4.0f);

    // Normalized result has unit length
    EXPECT_NEAR(norm.Length(), 1.0f, 0.0001f);
}

TEST(Vector3Test, IsNormalized_True)
{
    Vector3 v{ 0.0f, 1.0f, 0.0f };
    EXPECT_TRUE(v.IsNormalized());
}

TEST(Vector3Test, IsNormalized_False)
{
    Vector3 v{ 3.0f, 4.0f, 0.0f };
    EXPECT_FALSE(v.IsNormalized());
}

// =========================================================
// Cross Product
// =========================================================
TEST(Vector3Test, Cross_OrthogonalAxes)
{
    // Right × Up = Forward (X cross Y = +Z)
    Vector3 result = Vector3::Right().Cross(Vector3::Up());
    EXPECT_NEAR(result.x, 0.0f, 0.0001f);
    EXPECT_NEAR(result.y, 0.0f, 0.0001f);
    EXPECT_NEAR(result.z, 1.0f, 0.0001f);
}

TEST(Vector3Test, Cross_StaticVersion)
{
    Vector3 a{ 1.0f, 0.0f, 0.0f };
    Vector3 b{ 0.0f, 1.0f, 0.0f };
    Vector3 result = Vector3::Cross(a, b);

    EXPECT_NEAR(result.z, 1.0f, 0.0001f);
}

TEST(Vector3Test, Cross_ParallelVectors_IsZero)
{
    Vector3 a{ 1.0f, 0.0f, 0.0f };
    Vector3 b{ 3.0f, 0.0f, 0.0f };
    Vector3 result = a.Cross(b);

    EXPECT_NEAR(result.x, 0.0f, 0.0001f);
    EXPECT_NEAR(result.y, 0.0f, 0.0001f);
    EXPECT_NEAR(result.z, 0.0f, 0.0001f);
}

TEST(Vector3Test, Cross_AntiCommutative)
{
    Vector3 a{ 1.0f, 2.0f, 3.0f };
    Vector3 b{ 4.0f, 5.0f, 6.0f };

    Vector3 ab = a.Cross(b);
    Vector3 ba = b.Cross(a);

    EXPECT_NEAR(ab.x, -ba.x, 0.0001f);
    EXPECT_NEAR(ab.y, -ba.y, 0.0001f);
    EXPECT_NEAR(ab.z, -ba.z, 0.0001f);
}

// =========================================================
// Dot Product
// =========================================================
TEST(Vector3Test, Dot_OrthogonalVectors_IsZero)
{
    EXPECT_FLOAT_EQ(Vector3::Right().Dot(Vector3::Up()), 0.0f);
}

TEST(Vector3Test, Dot_ParallelVectors)
{
    Vector3 a{ 2.0f, 0.0f, 0.0f };
    Vector3 b{ 3.0f, 0.0f, 0.0f };
    EXPECT_FLOAT_EQ(a.Dot(b), 6.0f);
}

TEST(Vector3Test, Dot_OppositeVectors_Negative)
{
    EXPECT_FLOAT_EQ(Vector3::Up().Dot(Vector3::Down()), -1.0f);
}

TEST(Vector3Test, Dot_StaticVersion)
{
    Vector3 a{ 1.0f, 2.0f, 3.0f };
    Vector3 b{ 4.0f, 5.0f, 6.0f };
    EXPECT_FLOAT_EQ(Vector3::Dot(a, b), 32.0f);
}
