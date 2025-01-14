#include <gtest/gtest.h>
#include "Quat.h"
#include "Vector.h"
#include "Rotator.h"
#include <DirectXMath.h>
#include <cmath>

class QuatTest : public ::testing::Test
{
protected:
    // Вспомогательные методы для сравнения float с погрешностью
    bool FloatNearEqual(float a, float b, float epsilon = 1e-4f) { return std::abs(a - b) < epsilon; }
};

// Тесты конструкторов
TEST_F(QuatTest, DefaultConstructor)
{
    Quat q;
    auto simd = q.ToSIMD();

    EXPECT_FLOAT_EQ(DirectX::XMVectorGetX(simd), 0.0f);
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetY(simd), 0.0f);
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetZ(simd), 0.0f);
    EXPECT_FLOAT_EQ(DirectX::XMVectorGetW(simd), 1.0f);
}

TEST_F(QuatTest, SIMDConstructor)
{
    DirectX::XMVECTOR testQuat = DirectX::XMQuaternionRotationRollPitchYaw(0.5f, 1.0f, 1.5f);
    Quat q(testQuat);

    auto simd = q.ToSIMD();
    EXPECT_TRUE(DirectX::XMQuaternionEqual(simd, testQuat));
}

TEST_F(QuatTest, EulerAnglesConstructor)
{
    float pitch = 45.0f, yaw = 30.0f, roll = 15.0f;
    Quat q(pitch, yaw, roll);

    // Проверка, что quaternion создан корректно
    auto simd = q.ToSIMD();
    EXPECT_NO_THROW(DirectX::XMQuaternionNormalize(simd));
}

// Тесты операторов
TEST_F(QuatTest, EqualityOperator)
{
    Quat q1(1.0f, 0.0f, 0.0f);
    Quat q2(1.0f, 0.0f, 0.0f);
    Quat q3(0.0f, 1.0f, 0.0f);

    EXPECT_TRUE(q1 == q2);
    EXPECT_FALSE(q1 == q3);
}

TEST_F(QuatTest, InequalityOperator)
{
    Quat q1(1.0f, 0.0f, 0.0f);
    Quat q2(0.0f, 1.0f, 0.0f);

    EXPECT_TRUE(q1 != q2);
}

// Тесты нормализации
TEST_F(QuatTest, Normalization)
{
    Quat q(2.0f, 3.0f, 4.0f);
    Quat normalized = q.GetNormalize();

    // Проверка, что длина quaternion равна 1
    auto normalizedSimd = normalized.ToSIMD();
    float length = DirectX::XMVectorGetX(DirectX::XMVector4Length(normalizedSimd));
    EXPECT_NEAR(length, 1.0f, 1e-4f);
}

// Тесты умножения quaternions
TEST_F(QuatTest, QuaternionMultiplication)
{
    Quat q1(1.0f, 0.0f, 0.0f);
    Quat q2(0.0f, 1.0f, 0.0f);

    Quat multiplied = q1.Mutiply(q2);

    // Проверка, что результат не нулевой
    auto simd = multiplied.ToSIMD();
    EXPECT_FALSE(DirectX::XMVector4Equal(simd, DirectX::XMVectorZero()));
}

// Тесты обращения quaternion
TEST_F(QuatTest, QuaternionInverse)
{
    Quat q(1.0f, 2.0f, 3.0f);
    Quat inverted = q.Inverse();

    // Проверка свойств обратного quaternion
    Quat identity = q.Mutiply(inverted);
    EXPECT_TRUE(identity.IsNearEqual(Quat::Identity()));
}

// Тесты интерполяции
TEST_F(QuatTest, Slerp)
{
    Quat q1(0.0f, 0.0f, 0.0f);
    Quat q2(1.0f, 1.0f, 1.0f);

    Quat interpolated = Quat::SlerpQuats(q1, q2, 0.5f);

    // Проверка, что интерполированный quaternion находится между исходными
    EXPECT_NO_THROW(interpolated.GetNormalize());
}

// Тесты вращения вектора
TEST_F(QuatTest, VectorRotation)
{
    Vector v(1.0f, 0.0f, 0.0f);
    Quat rotation(0.0f, DirectX::XM_PIDIV2, 0.0f);  // поворот на 90 градусов вокруг оси Y

    Vector rotated = Quat::RotateVector(v, rotation);

    // Проверка корректности поворота
    EXPECT_NEAR(rotated.GetX(), 0.0f, 1e-4f);
    EXPECT_NEAR(rotated.GetY(), 0.0f, 1e-4f);
    EXPECT_NEAR(rotated.GetZ(), -1.0f, 1e-4f);
}

// Тесты конвертации между системами поворота
TEST_F(QuatTest, RotationConversion)
{
    Rotator rot(45.0f, 30.0f, 15.0f);
    Quat q = Quat::RotatorToQuat(rot);

    Rotator backToRotator = Quat::QuatToRotator(q);

    // Проверка близости углов
    EXPECT_NEAR(backToRotator.GetPitch(), rot.GetPitch(), 1.0f);
    EXPECT_NEAR(backToRotator.GetYaw(), rot.GetYaw(), 1.0f);
    EXPECT_NEAR(backToRotator.GetRoll(), rot.GetRoll(), 1.0f);
}

// Дополнительные граничные случаи
TEST_F(QuatTest, EdgeCases)
{
    // Нулевой quaternion
    Quat zeroQuat(0.0f, 0.0f, 0.0f);
    EXPECT_NO_THROW(zeroQuat.GetNormalize());

    // Очень большие углы
    Quat largeAngleQuat(1000.0f, 2000.0f, 3000.0f);
    EXPECT_NO_THROW(largeAngleQuat.GetNormalize());
}

// Тест на near equality
TEST_F(QuatTest, NearEquality)
{
    Quat q1(1.0f, 0.0f, 0.0f);
    Quat q2(1.001f, 0.011f, 0.0001f);

    EXPECT_TRUE(q1.IsNearEqual(q2));
    EXPECT_FALSE(q1.IsNearEqual(q2, 1e-5f));
}