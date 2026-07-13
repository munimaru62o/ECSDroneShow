#include "pch.h"
#include "Engine/Math/Color.h"

// =========================================================
// Construction / Defaults
// =========================================================
TEST(ColorTest, DefaultConstruction_IsWhite)
{
    Color c;
    EXPECT_FLOAT_EQ(c.r, 1.0f);
    EXPECT_FLOAT_EQ(c.g, 1.0f);
    EXPECT_FLOAT_EQ(c.b, 1.0f);
    EXPECT_FLOAT_EQ(c.a, 1.0f);
}

// =========================================================
// Lerp
// =========================================================
TEST(ColorTest, Lerp_T0_ReturnsStart)
{
    Color start = Color::Red();
    Color end = Color::Blue();
    Color result = Color::Lerp(start, end, 0.0f);

    EXPECT_FLOAT_EQ(result.r, 1.0f);
    EXPECT_FLOAT_EQ(result.g, 0.0f);
    EXPECT_FLOAT_EQ(result.b, 0.0f);
    EXPECT_FLOAT_EQ(result.a, 1.0f);
}

TEST(ColorTest, Lerp_T1_ReturnsEnd)
{
    Color start = Color::Red();
    Color end = Color::Blue();
    Color result = Color::Lerp(start, end, 1.0f);

    EXPECT_FLOAT_EQ(result.r, 0.0f);
    EXPECT_FLOAT_EQ(result.g, 0.0f);
    EXPECT_FLOAT_EQ(result.b, 1.0f);
    EXPECT_FLOAT_EQ(result.a, 1.0f);
}

TEST(ColorTest, Lerp_Midpoint)
{
    Color start = Color::Black();
    Color end = Color::White();
    Color result = Color::Lerp(start, end, 0.5f);

    EXPECT_NEAR(result.r, 0.5f, 0.001f);
    EXPECT_NEAR(result.g, 0.5f, 0.001f);
    EXPECT_NEAR(result.b, 0.5f, 0.001f);
    EXPECT_NEAR(result.a, 1.0f, 0.001f);
}

TEST(ColorTest, Lerp_ClampsBelow0)
{
    Color start = Color::Red();
    Color end = Color::Blue();
    Color result = Color::Lerp(start, end, -1.0f);

    // Should clamp to t=0, returning start
    EXPECT_FLOAT_EQ(result.r, 1.0f);
    EXPECT_FLOAT_EQ(result.b, 0.0f);
}

TEST(ColorTest, Lerp_ClampsAbove1)
{
    Color start = Color::Red();
    Color end = Color::Blue();
    Color result = Color::Lerp(start, end, 2.0f);

    // Should clamp to t=1, returning end
    EXPECT_FLOAT_EQ(result.r, 0.0f);
    EXPECT_FLOAT_EQ(result.b, 1.0f);
}

TEST(ColorTest, Lerp_AlphaChannel)
{
    Color start{ 1.0f, 1.0f, 1.0f, 0.0f };
    Color end{ 1.0f, 1.0f, 1.0f, 1.0f };
    Color result = Color::Lerp(start, end, 0.5f);

    EXPECT_NEAR(result.a, 0.5f, 0.001f);
}

TEST(ColorTest, Lerp_SameColor_ReturnsSame)
{
    Color c = Color::Cyan();
    Color result = Color::Lerp(c, c, 0.5f);

    EXPECT_FLOAT_EQ(result.r, c.r);
    EXPECT_FLOAT_EQ(result.g, c.g);
    EXPECT_FLOAT_EQ(result.b, c.b);
    EXPECT_FLOAT_EQ(result.a, c.a);
}
