#include <gtest/gtest.h>
#include <cmath>
#include "Vector.h"

    class VectorTest : public ::testing::Test
{
protected:
    Vector v1{1.0f, 2.0f, 3.0f};
    Vector v2{4.0f, 5.0f, 6.0f};
    const float epsilon = 1e-4f;
};

// Конструкторы и базовые операции
TEST_F(VectorTest, Constructors)
{
    Vector defaultVec;
    EXPECT_FLOAT_EQ(defaultVec.Data.x, 0.0f);
    EXPECT_FLOAT_EQ(defaultVec.Data.y, 0.0f);
    EXPECT_FLOAT_EQ(defaultVec.Data.z, 0.0f);

    Vector paramVec(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(paramVec.Data.x, 1.0f);
    EXPECT_FLOAT_EQ(paramVec.Data.y, 2.0f);
    EXPECT_FLOAT_EQ(paramVec.Data.z, 3.0f);
}

// Тесты арифметических операторов
TEST_F(VectorTest, Addition)
{
    Vector result = v1 + v2;
    EXPECT_FLOAT_EQ(result.Data.x, 5.0f);
    EXPECT_FLOAT_EQ(result.Data.y, 7.0f);
    EXPECT_FLOAT_EQ(result.Data.z, 9.0f);

    v1 += v2;
    EXPECT_FLOAT_EQ(v1.Data.x, 5.0f);
    EXPECT_FLOAT_EQ(v1.Data.y, 7.0f);
    EXPECT_FLOAT_EQ(v1.Data.z, 9.0f);
}

TEST_F(VectorTest, Subtraction)
{
    Vector result = v2 - v1;
    EXPECT_FLOAT_EQ(result.Data.x, 3.0f);
    EXPECT_FLOAT_EQ(result.Data.y, 3.0f);
    EXPECT_FLOAT_EQ(result.Data.z, 3.0f);

    v2 -= v1;
    EXPECT_FLOAT_EQ(v2.Data.x, 3.0f);
    EXPECT_FLOAT_EQ(v2.Data.y, 3.0f);
    EXPECT_FLOAT_EQ(v2.Data.z, 3.0f);
}

TEST_F(VectorTest, Multiplication)
{
    Vector result = v1 * 2.0f;
    EXPECT_FLOAT_EQ(result.Data.x, 2.0f);
    EXPECT_FLOAT_EQ(result.Data.y, 4.0f);
    EXPECT_FLOAT_EQ(result.Data.z, 6.0f);

    v1 *= 2.0f;
    EXPECT_FLOAT_EQ(v1.Data.x, 2.0f);
    EXPECT_FLOAT_EQ(v1.Data.y, 4.0f);
    EXPECT_FLOAT_EQ(v1.Data.z, 6.0f);
}

TEST_F(VectorTest, Division)
{
    Vector result = v2 / 2.0f;
    EXPECT_FLOAT_EQ(result.Data.x, 2.0f);
    EXPECT_FLOAT_EQ(result.Data.y, 2.5f);
    EXPECT_FLOAT_EQ(result.Data.z, 3.0f);

    v2 /= 2.0f;
    EXPECT_FLOAT_EQ(v2.Data.x, 2.0f);
    EXPECT_FLOAT_EQ(v2.Data.y, 2.5f);
    EXPECT_FLOAT_EQ(v2.Data.z, 3.0f);
}

// Тесты сравнения
TEST_F(VectorTest, Comparison)
{
    Vector v3{1.0f, 2.0f, 3.0f};

    EXPECT_TRUE(v1 == v3);
    EXPECT_FALSE(v1 != v3);
    EXPECT_TRUE(v1 != v2);
}

TEST_F(VectorTest, NearEqual)
{
    Vector v3{1.0011f, 2.0011f, 3.0005f};
    EXPECT_TRUE(v1.NearEqual(v3));

    Vector v4{1.1f, 2.0f, 3.0f};
    EXPECT_FALSE(v1.NearEqual(v4));
}

// Тесты математических операций
TEST_F(VectorTest, Length)
{
    EXPECT_NEAR(v1.Length(), std::sqrt(14.0f), epsilon);
    EXPECT_NEAR(v1.LengthSq(), 14.0f, epsilon);
    EXPECT_NEAR(v1.LengthEst(), std::sqrt(14.0f), 0.01f);
}

TEST_F(VectorTest, Normalize)
{
    Vector normalized = v1.Normalize();
    EXPECT_NEAR(normalized.Length(), 1.0f, epsilon);
}

TEST_F(VectorTest, DotProduct)
{
    float dot = v1.Dot(v2);
    EXPECT_NEAR(dot, 32.0f, epsilon);
}

TEST_F(VectorTest, CrossProduct)
{
    Vector cross = v1.Cross(v2);
    Vector expected{-3.0f, 6.0f, -3.0f};
    EXPECT_TRUE(cross.NearEqual(expected));
}

TEST_F(VectorTest, Angle)
{
    Vector v3{1.0f, 0.0f, 0.0f};
    Vector v4{0.0f, 1.0f, 0.0f};
    EXPECT_NEAR(v3.Angle(v4), 1.570796327f, epsilon);
}

TEST_F(VectorTest, Abs)
{
    Vector negVec{-1.0f, -2.0f, -3.0f};
    Vector absVec = negVec.Abs();

    EXPECT_FLOAT_EQ(absVec.Data.x, 1.0f);
    EXPECT_FLOAT_EQ(absVec.Data.y, 2.0f);
    EXPECT_FLOAT_EQ(absVec.Data.z, 3.0f);
}

TEST_F(VectorTest, Clamp)
{
    Vector min{0.0f, 0.0f, 0.0f};
    Vector max{2.0f, 2.0f, 2.0f};
    Vector clamped = v1.Clamp(min, max);

    EXPECT_FLOAT_EQ(clamped.Data.x, 1.0f);
    EXPECT_FLOAT_EQ(clamped.Data.y, 2.0f);
    EXPECT_FLOAT_EQ(clamped.Data.z, 2.0f);
}

TEST_F(VectorTest, Distance)
{
    EXPECT_NEAR(Vector::Distance(v1, v2), std::sqrt(27.0f), epsilon);
}

TEST_F(VectorTest, Lerp)
{
    Vector start{0.0f, 0.0f, 0.0f};
    Vector end{1.0f, 1.0f, 1.0f};

    Vector mid = Vector::Lerp(start, end, 0.5f);
    EXPECT_TRUE(mid.NearEqual(Vector{0.5f, 0.5f, 0.5f}));
}

// Статические методы
TEST_F(VectorTest, StaticMethods)
{
    Vector zero = Vector::Zero();
    Vector rhsZero(0.0f, 0.0f, 0.0f);
    EXPECT_TRUE(zero == rhsZero);

    Vector one = Vector::One();
    Vector rhsOne(1.0f, 1.0f, 1.0f);
    EXPECT_TRUE(one == rhsOne);

    
    Vector up = Vector::Up();
    Vector rhsUp(0.0f, 1.0f, 0.0f);
    EXPECT_TRUE(up == rhsUp);
}

// Тесты сеттеров
TEST_F(VectorTest, Setters)
{
    Vector v;
    v.SetX(1.0f);
    v.SetY(2.0f);
    v.SetZ(3.0f);

    EXPECT_FLOAT_EQ(v.Data.x, 1.0f);
    EXPECT_FLOAT_EQ(v.Data.y, 2.0f);
    EXPECT_FLOAT_EQ(v.Data.z, 3.0f);
}

// Тест производительности (опционально)
TEST_F(VectorTest, Performance)
{
    const int iterations = 1000000;
    Vector result;

    // Замер производительности сложения
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        result = v1 + v2;
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Addition Performance: " << duration.count() << " microseconds" << std::endl;
}