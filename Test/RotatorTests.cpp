#include <gtest/gtest.h>
#include "Rotator.h"
#include "Vector.h"

class RotatorTest : public ::testing::Test
{
protected:
    static constexpr float EPSILON = 0.001f;

    static bool IsNearEqual(float a, float b, float epsilon = EPSILON) { return std::abs(a - b) < epsilon; }

    static bool VectorNearEqual(const Vector& v1, const Vector& v2, float epsilon = EPSILON)
    {
        return IsNearEqual(v1.GetX(), v2.GetX(), epsilon) && IsNearEqual(v1.GetY(), v2.GetY(), epsilon) &&
               IsNearEqual(v1.GetZ(), v2.GetZ(), epsilon);
    }
};

// Конструкторы и базовая инициализация
TEST_F(RotatorTest, Constructors)
{
    // Конструктор по умолчанию
    Rotator r1;
    EXPECT_FLOAT_EQ(r1.GetPitch(), 0.0f);
    EXPECT_FLOAT_EQ(r1.GetYaw(), 0.0f);
    EXPECT_FLOAT_EQ(r1.GetRoll(), 0.0f);

    // Конструктор с параметрами
    Rotator r2(30.0f, 45.0f, 60.0f);
    EXPECT_FLOAT_EQ(r2.GetPitch(), 30.0f);
    EXPECT_FLOAT_EQ(r2.GetYaw(), 45.0f);
    EXPECT_FLOAT_EQ(r2.GetRoll(), 60.0f);

    // Конструктор из XMFLOAT3A
    DirectX::XMFLOAT3A float3a{10.0f, 20.0f, 30.0f};
    Rotator r3(float3a);
    EXPECT_FLOAT_EQ(r3.GetPitch(), 10.0f);
    EXPECT_FLOAT_EQ(r3.GetYaw(), 20.0f);
    EXPECT_FLOAT_EQ(r3.GetRoll(), 30.0f);

    // Конструктор копирования
    Rotator r4(r2);
    EXPECT_TRUE(r2 == r4);

    // Конструктор перемещения
    Rotator r5(std::move(r4));
    EXPECT_TRUE(r2 == r5);
    EXPECT_TRUE(r4 == Rotator());  // r4 должен быть обнулен
}

// Операторы присваивания
TEST_F(RotatorTest, AssignmentOperators)
{
    Rotator r1(30.0f, 45.0f, 60.0f);

    // Оператор копирования
    Rotator r2;
    r2 = r1;
    EXPECT_TRUE(r1 == r2);

    // Оператор перемещения
    Rotator r3;
    r3 = std::move(r2);
    EXPECT_TRUE(r1 == r3);
    EXPECT_TRUE(r2 == Rotator());  // r2 должен быть обнулен
}

// Геттеры и сеттеры
TEST_F(RotatorTest, GettersSetters)
{
    Rotator r;

    r.SetPitch(30.0f);
    EXPECT_FLOAT_EQ(r.GetPitch(), 30.0f);

    r.SetYaw(45.0f);
    EXPECT_FLOAT_EQ(r.GetYaw(), 45.0f);

    r.SetRoll(60.0f);
    EXPECT_FLOAT_EQ(r.GetRoll(), 60.0f);

    auto data = r.GetData();
    EXPECT_FLOAT_EQ(data.x, 30.0f);
    EXPECT_FLOAT_EQ(data.y, 45.0f);
    EXPECT_FLOAT_EQ(data.z, 60.0f);
}

// Арифметические операторы
TEST_F(RotatorTest, ArithmeticOperators)
{
    Rotator r1(10.0f, 20.0f, 30.0f);
    Rotator r2(5.0f, 10.0f, 15.0f);

    // Сложение
    auto sum = r1 + r2;
    EXPECT_TRUE(sum == Rotator(15.0f, 30.0f, 45.0f));

    // Вычитание
    auto diff = r1 - r2;
    EXPECT_TRUE(diff == Rotator(5.0f, 10.0f, 15.0f));

    // Умножение на скаляр
    auto mult = r1 * 2.0f;
    EXPECT_TRUE(mult == Rotator(20.0f, 40.0f, 60.0f));

    // Деление на скаляр
    auto div = r1 / 2.0f;
    EXPECT_TRUE(div == Rotator(5.0f, 10.0f, 15.0f));

    // Деление на ноль
    auto divZero = r1 / 0.0f;
    EXPECT_TRUE(divZero == Rotator::Zero());
}

// Составные операторы присваивания
TEST_F(RotatorTest, CompoundAssignmentOperators)
{
    Rotator r1(10.0f, 20.0f, 30.0f);
    Rotator r2(5.0f, 10.0f, 15.0f);

    auto r3 = r1;
    r3 += r2;
    EXPECT_TRUE(r3 == Rotator(15.0f, 30.0f, 45.0f));

    r3 = r1;
    r3 -= r2;
    EXPECT_TRUE(r3 == Rotator(5.0f, 10.0f, 15.0f));

    r3 = r1;
    r3 *= 2.0f;
    EXPECT_TRUE(r3 == Rotator(20.0f, 40.0f, 60.0f));

    r3 = r1;
    r3 /= 2.0f;
    EXPECT_TRUE(r3 == Rotator(5.0f, 10.0f, 15.0f));
}

// Операторы сравнения и IsNearEqual
TEST_F(RotatorTest, ComparisonOperators)
{
    Rotator r1(10.0f, 20.0f, 30.0f);
    Rotator r2(10.0f, 20.0f, 30.0f);
    Rotator r3(10.1f, 20.1f, 30.1f);

    EXPECT_TRUE(r1 == r2);
    EXPECT_FALSE(r1 == r3);
    EXPECT_TRUE(r1 != r3);
    EXPECT_FALSE(r1 != r2);

    EXPECT_TRUE(r1.IsNearEqual(r2));
    EXPECT_TRUE(r1.IsNearEqual(r3, 0.2f));
    EXPECT_FALSE(r1.IsNearEqual(r3, 0.01f));
}

// Нормализация углов
TEST_F(RotatorTest, AngleNormalization)
{
    // Normalize180
    std::vector<std::pair<float, float>> testCases180 = {
        {0.0f, 0.0f}, {180.0f, 180.0f}, {-180.0f, -180.0f}, {360.0f, 0.0f}, {540.0f, -180.0f}, {-360.0f, 0.0f}, {-540.0f, 180.0f}};

    for (const auto& [input, expected] : testCases180)
    {
        Rotator r(input, input, input);
        Rotator normalized = r.Normalize180();
        EXPECT_TRUE(IsNearEqual(normalized.GetPitch(), expected)) << "Failed for input: " << input;
    }

    // Normalize360
    std::vector<std::pair<float, float>> testCases360 = {
        {0.0f, 0.0f}, {360.0f, 0.0f}, {720.0f, 0.0f}, {-360.0f, 0.0f}, {-720.0f, 0.0f}, {180.0f, 180.0f}, {-180.0f, 180.0f}};

    for (const auto& [input, expected] : testCases360)
    {
        Rotator r(input, input, input);
        Rotator normalized = r.Normalize360();
        EXPECT_TRUE(IsNearEqual(normalized.GetPitch(), expected)) << "Failed for input: " << input;
    }
}

// Конвертация кватернионов
TEST_F(RotatorTest, QuaternionConversion)
{
    std::vector<Rotator> testRotators = {Rotator(0.0f, 0.0f, 0.0f), Rotator(90.0f, 0.0f, 0.0f), Rotator(0.0f, 90.0f, 0.0f),
        Rotator(0.0f, 0.0f, 90.0f), Rotator(30.0f, 45.0f, 60.0f)};

    for (const auto& original : testRotators)
    {
        auto quat = original.ToQuaternion();
        auto result = Rotator::FromQuaternion(quat);

        // Нормализуем углы перед сравнением
        auto normalizedOriginal = original.Normalize180();
        auto normalizedResult = result.Normalize180();

        EXPECT_TRUE(normalizedResult.IsNearEqual(normalizedOriginal, 0.1f))
            << "Failed for rotation: " << original.GetPitch() << ", " << original.GetYaw() << ", " << original.GetRoll();
    }
}

// Векторы направления
TEST_F(RotatorTest, DirectionVectors)
{
    // Проверка базовых векторов без поворота
    Rotator zeroRot;

    auto forward = zeroRot.GetForwardVector();
    auto right = zeroRot.GetRightVector();
    auto up = zeroRot.GetUpVector();

    EXPECT_TRUE(VectorNearEqual(forward, Vector(0.0f, 0.0f, 1.0f)));
    EXPECT_TRUE(VectorNearEqual(right, Vector(1.0f, 0.0f, 0.0f)));
    EXPECT_TRUE(VectorNearEqual(up, Vector(0.0f, 1.0f, 0.0f)));

    // Проверка с поворотом на 90 градусов вокруг Y
    Rotator yaw90(0.0f, 90.0f, 0.0f);
    auto rotatedForward = yaw90.GetForwardVector();
    EXPECT_TRUE(VectorNearEqual(rotatedForward, Vector(-1.0f, 0.0f, 0.0f)));
}

// Клампинг
TEST_F(RotatorTest, Clamping)
{
    Rotator value(0.0f, 45.0f, 90.0f);
    Rotator min(-30.0f, 0.0f, 0.0f);
    Rotator max(30.0f, 60.0f, 180.0f);

    auto clamped = value.Clamp(min, max);
    EXPECT_FLOAT_EQ(clamped.GetPitch(), 0.0f);
    EXPECT_FLOAT_EQ(clamped.GetYaw(), 45.0f);
    EXPECT_FLOAT_EQ(clamped.GetRoll(), 90.0f);

    Rotator outOfBounds(100.0f, -50.0f, 200.0f);
    auto clampedOutOfBounds = outOfBounds.Clamp(min, max);
    EXPECT_FLOAT_EQ(clampedOutOfBounds.GetPitch(), 30.0f);
    EXPECT_FLOAT_EQ(clampedOutOfBounds.GetYaw(), 0.0f);
    EXPECT_FLOAT_EQ(clampedOutOfBounds.GetRoll(), 180.0f);
}

// Интерполяция
TEST_F(RotatorTest, Interpolation)
{
    Rotator start(0.0f, 0.0f, 0.0f);
    Rotator end(90.0f, 90.0f, 90.0f);

    auto result0 = Rotator::Lerp(start, end, 0.0f);
    EXPECT_TRUE(result0.IsNearEqual(start));

    auto result1 = Rotator::Lerp(start, end, 1.0f);
    EXPECT_TRUE(result1.IsNearEqual(end));

    auto result05 = Rotator::Lerp(start, end, 0.5f);
    EXPECT_TRUE(IsNearEqual(result05.GetPitch(), 45.0f));
    EXPECT_TRUE(IsNearEqual(result05.GetYaw(), 45.0f));
    EXPECT_TRUE(IsNearEqual(result05.GetRoll(), 45.0f));
}

// Нормализация осей
TEST_F(RotatorTest, AxisNormalization)
{
    struct TestCase
    {
        float input;
        float expectedNormal;
        float expectedClamped;
    };

    std::vector<TestCase> testCases = {{0.0f, 0.0f, 0.0f}, {180.0f, 180.0f, 180.0f}, {360.0f, 0.0f, 0.0f}, {-180.0f, -180.0f, 180.0f},
        {-360.0f, 0.0f, 0.0f}, {720.0f, 0.0f, 0.0f}, {-720.0f, 0.0f, 0.0f}, {45.0f, 45.0f, 45.0f}, {225.0f, -135.0f, 225.0f},
        {-45.0f, -45.0f, 315.0f}};

    for (const auto& tc : testCases)
    {
        float normalized = Rotator::NormalizeAxis(tc.input);
        float clamped = Rotator::ClampAxis(tc.input);

        EXPECT_NEAR(normalized, tc.expectedNormal, EPSILON) << "NormalizeAxis failed for input: " << tc.input;
        EXPECT_NEAR(clamped, tc.expectedClamped, EPSILON) << "ClampAxis failed for input: " << tc.input;
    }
}