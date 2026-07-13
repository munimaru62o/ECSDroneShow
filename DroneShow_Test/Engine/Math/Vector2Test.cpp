#include "pch.h"
#include "Engine/Math/Vector2.h"

// =========================================================
// Construction / Static Helpers
// =========================================================
TEST(Vector2Test, DefaultConstruction_IsZero)
{
    Vector2 v;
    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
}

TEST(Vector2Test, StaticHelpers_ReturnCorrectDirections)
{
    EXPECT_FLOAT_EQ(Vector2::Up().y, 1.0f);
    EXPECT_FLOAT_EQ(Vector2::Down().y, -1.0f);
    EXPECT_FLOAT_EQ(Vector2::Right().x, 1.0f);
    EXPECT_FLOAT_EQ(Vector2::Left().x, -1.0f);

    Vector2 zero = Vector2::Zero();
    EXPECT_FLOAT_EQ(zero.x, 0.0f);
    EXPECT_FLOAT_EQ(zero.y, 0.0f);
}

// =========================================================
// Arithmetic Operators
// =========================================================
TEST(Vector2Test, Addition)
{
    Vector2 a{ 1.0f, 2.0f };
    Vector2 b{ 4.0f, 5.0f };
    Vector2 result = a + b;

    EXPECT_FLOAT_EQ(result.x, 5.0f);
    EXPECT_FLOAT_EQ(result.y, 7.0f);
}

TEST(Vector2Test, AdditionAssignment)
{
    Vector2 a{ 1.0f, 2.0f };
    a += Vector2{ 10.0f, 20.0f };

    EXPECT_FLOAT_EQ(a.x, 11.0f);
    EXPECT_FLOAT_EQ(a.y, 22.0f);
}

TEST(Vector2Test, Subtraction)
{
    Vector2 a{ 5.0f, 7.0f };
    Vector2 b{ 1.0f, 2.0f };
    Vector2 result = a - b;

    EXPECT_FLOAT_EQ(result.x, 4.0f);
    EXPECT_FLOAT_EQ(result.y, 5.0f);
}

TEST(Vector2Test, SubtractionAssignment)
{
    Vector2 a{ 10.0f, 20.0f };
    a -= Vector2{ 1.0f, 2.0f };

    EXPECT_FLOAT_EQ(a.x, 9.0f);
    EXPECT_FLOAT_EQ(a.y, 18.0f);
}

TEST(Vector2Test, UnaryNegation)
{
    Vector2 v{ 1.0f, -2.0f };
    Vector2 neg = -v;

    EXPECT_FLOAT_EQ(neg.x, -1.0f);
    EXPECT_FLOAT_EQ(neg.y, 2.0f);
}

TEST(Vector2Test, ScalarMultiplication)
{
    Vector2 v{ 2.0f, 3.0f };
    Vector2 result = v * 3.0f;

    EXPECT_FLOAT_EQ(result.x, 6.0f);
    EXPECT_FLOAT_EQ(result.y, 9.0f);
}

TEST(Vector2Test, ScalarMultiplicationAssignment)
{
    Vector2 v{ 2.0f, 3.0f };
    v *= 0.5f;

    EXPECT_FLOAT_EQ(v.x, 1.0f);
    EXPECT_FLOAT_EQ(v.y, 1.5f);
}

TEST(Vector2Test, ScalarDivision)
{
    Vector2 v{ 10.0f, 20.0f };
    Vector2 result = v / 2.0f;

    EXPECT_FLOAT_EQ(result.x, 5.0f);
    EXPECT_FLOAT_EQ(result.y, 10.0f);
}

TEST(Vector2Test, ScalarDivisionAssignment)
{
    Vector2 v{ 12.0f, 6.0f };
    v /= 3.0f;

    EXPECT_FLOAT_EQ(v.x, 4.0f);
    EXPECT_FLOAT_EQ(v.y, 2.0f);
}

// =========================================================
// Length / Distance
// =========================================================
TEST(Vector2Test, LengthSq)
{
    Vector2 v{ 3.0f, 4.0f };
    EXPECT_FLOAT_EQ(v.LengthSq(), 25.0f);
}

TEST(Vector2Test, Length)
{
    Vector2 v{ 3.0f, 4.0f };
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);
}

TEST(Vector2Test, Length_ZeroVector)
{
    Vector2 v = Vector2::Zero();
    EXPECT_FLOAT_EQ(v.Length(), 0.0f);
}

TEST(Vector2Test, DistanceSq)
{
    Vector2 a{ 1.0f, 0.0f };
    Vector2 b{ 4.0f, 4.0f };
    EXPECT_FLOAT_EQ(Vector2::DistanceSq(a, b), 25.0f); // 3^2 + 4^2 = 9 + 16 = 25
}

TEST(Vector2Test, Distance)
{
    Vector2 a{ 1.0f, 0.0f };
    Vector2 b{ 4.0f, 4.0f };
    EXPECT_FLOAT_EQ(Vector2::Distance(a, b), 5.0f);
}

TEST(Vector2Test, Distance_SamePoint_IsZero)
{
    Vector2 a{ 5.0f, 5.0f };
    EXPECT_FLOAT_EQ(Vector2::Distance(a, a), 0.0f);
}

// =========================================================
// Normalize
// =========================================================
TEST(Vector2Test, Normalize_UnitLength)
{
    Vector2 v{ 3.0f, 4.0f };
    v.Normalize();

    EXPECT_NEAR(v.Length(), 1.0f, 0.0001f);
    EXPECT_NEAR(v.x, 0.6f, 0.0001f);
    EXPECT_NEAR(v.y, 0.8f, 0.0001f);
}

TEST(Vector2Test, Normalize_ZeroVector_StaysZero)
{
    Vector2 v = Vector2::Zero();
    v.Normalize();

    EXPECT_FLOAT_EQ(v.x, 0.0f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
}

TEST(Vector2Test, Normalized_DoesNotModifyOriginal)
{
    Vector2 original{ 3.0f, 4.0f };
    Vector2 norm = original.Normalized();

    // Original is unchanged
    EXPECT_FLOAT_EQ(original.x, 3.0f);
    EXPECT_FLOAT_EQ(original.y, 4.0f);

    // Normalized result has unit length
    EXPECT_NEAR(norm.Length(), 1.0f, 0.0001f);
}

TEST(Vector2Test, IsNormalized_True)
{
    Vector2 v{ 0.0f, 1.0f };
    EXPECT_TRUE(v.IsNormalized());
}

TEST(Vector2Test, IsNormalized_False)
{
    Vector2 v{ 3.0f, 4.0f };
    EXPECT_FALSE(v.IsNormalized());
}

// =========================================================
// Dot / Cross Product
// =========================================================
TEST(Vector2Test, Dot_OrthogonalVectors_IsZero)
{
    EXPECT_FLOAT_EQ(Vector2::Right().Dot(Vector2::Up()), 0.0f);
}

TEST(Vector2Test, Dot_StaticVersion)
{
    Vector2 a{ 1.0f, 2.0f };
    Vector2 b{ 4.0f, 5.0f };
    // 1*4 + 2*5 = 14
    EXPECT_FLOAT_EQ(Vector2::Dot(a, b), 14.0f);
}

TEST(Vector2Test, Cross_StaticVersion)
{
    Vector2 a{ 1.0f, 0.0f };
    Vector2 b{ 0.0f, 1.0f };
    // 1*1 - 0*0 = 1
    EXPECT_FLOAT_EQ(Vector2::Cross(a, b), 1.0f);
}

TEST(Vector2Test, Cross_ParallelVectors_IsZero)
{
    Vector2 a{ 1.0f, 2.0f };
    Vector2 b{ 2.0f, 4.0f };
    EXPECT_FLOAT_EQ(a.Cross(b), 0.0f);
}
