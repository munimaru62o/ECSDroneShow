#include "pch.h"
#include "Engine/Math/Matrix4.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Constants.h"

#include <cmath>

// =========================================================
// Identity
// =========================================================
TEST(Matrix4Test, Identity_DiagonalOnes)
{
    Matrix4 id = Matrix4::Identity();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                EXPECT_FLOAT_EQ(id.m[i][j], 1.0f);
            } else {
                EXPECT_FLOAT_EQ(id.m[i][j], 0.0f);
            }
        }
    }
}

// =========================================================
// Multiplication
// =========================================================
TEST(Matrix4Test, Multiply_Identity_PreservesMatrix)
{
    Matrix4 id = Matrix4::Identity();
    Matrix4 t = Matrix4::Translation(Vector3{ 5.0f, 10.0f, 15.0f });
    Matrix4 result = t * id;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(result.m[i][j], t.m[i][j], 0.0001f);
        }
    }
}

TEST(Matrix4Test, MultiplyAssignment)
{
    Matrix4 a = Matrix4::Translation(Vector3{ 1.0f, 2.0f, 3.0f });
    Matrix4 b = Matrix4::Scale(Vector3{ 2.0f, 2.0f, 2.0f });

    Matrix4 expected = a * b;
    a *= b;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(a.m[i][j], expected.m[i][j], 0.0001f);
        }
    }
}

// =========================================================
// Translation
// =========================================================
TEST(Matrix4Test, Translation_SetsCorrectValues)
{
    Matrix4 t = Matrix4::Translation(Vector3{ 10.0f, 20.0f, 30.0f });

    // Translation is stored in row 3 (row-major: m[3][0], m[3][1], m[3][2])
    EXPECT_FLOAT_EQ(t.m[3][0], 10.0f);
    EXPECT_FLOAT_EQ(t.m[3][1], 20.0f);
    EXPECT_FLOAT_EQ(t.m[3][2], 30.0f);
    EXPECT_FLOAT_EQ(t.m[3][3], 1.0f);

    // Diagonal should be 1
    EXPECT_FLOAT_EQ(t.m[0][0], 1.0f);
    EXPECT_FLOAT_EQ(t.m[1][1], 1.0f);
    EXPECT_FLOAT_EQ(t.m[2][2], 1.0f);
}

TEST(Matrix4Test, Translation_Zero_IsIdentity)
{
    Matrix4 t = Matrix4::Translation(Vector3::Zero());
    Matrix4 id = Matrix4::Identity();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_FLOAT_EQ(t.m[i][j], id.m[i][j]);
        }
    }
}

// =========================================================
// Scale
// =========================================================
TEST(Matrix4Test, Scale_SetsCorrectValues)
{
    Matrix4 s = Matrix4::Scale(Vector3{ 2.0f, 3.0f, 4.0f });

    EXPECT_FLOAT_EQ(s.m[0][0], 2.0f);
    EXPECT_FLOAT_EQ(s.m[1][1], 3.0f);
    EXPECT_FLOAT_EQ(s.m[2][2], 4.0f);
    EXPECT_FLOAT_EQ(s.m[3][3], 1.0f);
}

TEST(Matrix4Test, Scale_Uniform_ScalesDiagonal)
{
    Matrix4 s = Matrix4::Scale(Vector3{ 5.0f, 5.0f, 5.0f });

    EXPECT_FLOAT_EQ(s.m[0][0], 5.0f);
    EXPECT_FLOAT_EQ(s.m[1][1], 5.0f);
    EXPECT_FLOAT_EQ(s.m[2][2], 5.0f);
}

TEST(Matrix4Test, Scale_One_IsIdentity)
{
    Matrix4 s = Matrix4::Scale(Vector3{ 1.0f, 1.0f, 1.0f });
    Matrix4 id = Matrix4::Identity();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(s.m[i][j], id.m[i][j], 0.0001f);
        }
    }
}

// =========================================================
// Rotation
// =========================================================
TEST(Matrix4Test, Rotation_Identity_IsIdentity)
{
    Matrix4 r = Matrix4::Rotation(Quaternion::Identity());
    Matrix4 id = Matrix4::Identity();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(r.m[i][j], id.m[i][j], 0.0001f);
        }
    }
}

TEST(Matrix4Test, Rotation_90DegreesY)
{
    Quaternion q = Quaternion::FromAxisAngle(Vector3::Up(), MathConstants::PI * 0.5f);
    Matrix4 r = Matrix4::Rotation(q);

    // After 90-degree Y rotation (row-major):
    // Row 0 (X axis): should map to approximately {0, 0, -1}
    EXPECT_NEAR(r.m[0][0], 0.0f, 0.001f);
    EXPECT_NEAR(r.m[0][2], -1.0f, 0.001f);
}

// =========================================================
// TRS
// =========================================================
TEST(Matrix4Test, TRS_IdentityRotationUnitScale_IsTranslation)
{
    Vector3 pos{ 10.0f, 20.0f, 30.0f };
    Matrix4 trs = Matrix4::TRS(pos, Quaternion::Identity(), Vector3{ 1.0f, 1.0f, 1.0f });
    Matrix4 t = Matrix4::Translation(pos);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(trs.m[i][j], t.m[i][j], 0.0001f);
        }
    }
}

TEST(Matrix4Test, TRS_MatchesSeparateMultiplication)
{
    Vector3 pos{ 5.0f, 10.0f, 15.0f };
    Quaternion rot = Quaternion::FromAxisAngle(Vector3::Up(), 0.5f);
    Vector3 scale{ 2.0f, 3.0f, 4.0f };

    Matrix4 trs = Matrix4::TRS(pos, rot, scale);

    // S * R * T (in row-major, applied left to right: Scale first, then Rotate, then Translate)
    Matrix4 separate = Matrix4::Scale(scale) * Matrix4::Rotation(rot) * Matrix4::Translation(pos);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            EXPECT_NEAR(trs.m[i][j], separate.m[i][j], 0.001f);
        }
    }
}

// =========================================================
// LookAt
// =========================================================
TEST(Matrix4Test, LookAt_LookingForward)
{
    Vector3 eye{ 0.0f, 0.0f, 0.0f };
    Vector3 target{ 0.0f, 0.0f, 1.0f };
    Matrix4 view = Matrix4::LookAt(eye, target, Vector3::Up());

    // Z-axis column should point forward
    EXPECT_NEAR(view.m[2][2], 1.0f, 0.001f);
}

TEST(Matrix4Test, LookAt_EyeAtOrigin_TranslationIsZero)
{
    Vector3 eye{ 0.0f, 0.0f, 0.0f };
    Vector3 target{ 0.0f, 0.0f, 1.0f };
    Matrix4 view = Matrix4::LookAt(eye, target, Vector3::Up());

    // Translation part should be zero since eye is at origin
    EXPECT_NEAR(view.m[3][0], 0.0f, 0.001f);
    EXPECT_NEAR(view.m[3][1], 0.0f, 0.001f);
    EXPECT_NEAR(view.m[3][2], 0.0f, 0.001f);
}

// =========================================================
// Perspective
// =========================================================
TEST(Matrix4Test, Perspective_BasicProperties)
{
    Matrix4 p = Matrix4::Perspective(60.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

    // m[0][0] and m[1][1] should be positive (focal length)
    EXPECT_GT(p.m[0][0], 0.0f);
    EXPECT_GT(p.m[1][1], 0.0f);

    // m[2][3] should be 1 (perspective division flag)
    EXPECT_FLOAT_EQ(p.m[2][3], 1.0f);

    // m[3][3] should be 0 (perspective projection)
    EXPECT_FLOAT_EQ(p.m[3][3], 0.0f);
}

TEST(Matrix4Test, Perspective_AspectRatio_AffectsM00)
{
    Matrix4 wide = Matrix4::Perspective(60.0f, 2.0f, 0.1f, 100.0f);
    Matrix4 narrow = Matrix4::Perspective(60.0f, 0.5f, 0.1f, 100.0f);

    // Wider aspect ratio should have smaller m[0][0]
    EXPECT_LT(wide.m[0][0], narrow.m[0][0]);

    // m[1][1] should be the same (same FOV)
    EXPECT_NEAR(wide.m[1][1], narrow.m[1][1], 0.0001f);
}
