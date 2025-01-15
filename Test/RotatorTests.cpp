#include <gtest/gtest.h>
#include <DirectXMath.h>
#include "Rotator.h"
#include <iostream>
#include <iomanip>
#include "Vector.h"
#include "Quat.h"

class RotatorTest : public ::testing::Test
{
protected:
    Rotator rotator;

    // Вспомогательная функция для вывода кватерниона
    void PrintQuaternion(const char* name, DirectX::XMVECTOR quat) const
    {
        float x = DirectX::XMVectorGetX(quat);
        float y = DirectX::XMVectorGetY(quat);
        float z = DirectX::XMVectorGetZ(quat);
        float w = DirectX::XMVectorGetW(quat);

        std::cout << std::fixed << std::setprecision(6) << name << ": "
                  << "x=" << x << ", "
                  << "y=" << y << ", "
                  << "z=" << z << ", "
                  << "w=" << w << std::endl;
    }

protected:
    static constexpr float EPSILON = 0.001f;

    static bool IsNearEqual(float a, float b, float epsilon = EPSILON) { return std::abs(a - b) < epsilon; }

    static bool VectorNearEqual(const Vector& v1, const Vector& v2, float epsilon = EPSILON)
    {
        return IsNearEqual(v1.GetX(), v2.GetX(), epsilon) && IsNearEqual(v1.GetY(), v2.GetY(), epsilon) &&
               IsNearEqual(v1.GetZ(), v2.GetZ(), epsilon);
    }

    void PrintVectorDebugInfo(const char* name, const Vector& expected, const Vector& actual, const Rotator& rot)
    {
        std::cout << "Test for Rotator: Pitch=" << rot.GetPitch() << ", Yaw=" << rot.GetYaw() << ", Roll=" << rot.GetRoll() << std::endl;
        std::cout << name << " Vector:" << std::endl;
        std::cout << "  Expected: (" << expected.GetX() << ", " << expected.GetY() << ", " << expected.GetZ() << ")" << std::endl;
        std::cout << "  Actual:   (" << actual.GetX() << ", " << actual.GetY() << ", " << actual.GetZ() << ")" << std::endl;

        // Вычисление разницы
        Vector diff = expected - actual;
        std::cout << "  Difference: (" << diff.GetX() << ", " << diff.GetY() << ", " << diff.GetZ() << ")" << std::endl;
        std::cout << "  Magnitude of Difference: " << diff.Length() << std::endl;
        std::cout << "  Dot Product: " << expected.Dot(actual) << std::endl;
        std::cout << "  Angle between vectors: " << expected.Angle(actual) << " radians" << std::endl;
    }

    // Вспомогательная функция для сравнения векторов с расширенной отладкой
    bool VectorNearEqualWithDebug(const Vector& v1, const Vector& v2, const Rotator& rot, const char* vectorName, float epsilon = 0.001f)
    {
        bool isNearEqual = VectorNearEqual(v1, v2, epsilon);
        if (!isNearEqual)
        {
            PrintVectorDebugInfo(vectorName, v1, v2, rot);
        }
        return isNearEqual;
    }
};

TEST_F(RotatorTest, QuaternionConversion)
{
    struct TestCase
    {
        Rotator input;
        const char* description;
    };

    std::vector<TestCase> testCases = {{Rotator(0.0f, 0.0f, 0.0f), "Zero rotation"}, {Rotator(90.0f, 0.0f, 0.0f), "90 degrees pitch"},
        {Rotator(-90.0f, 0.0f, 0.0f), "-90 degrees pitch"}, {Rotator(0.0f, 90.0f, 0.0f), "90 degrees yaw"},
        {Rotator(0.0f, -90.0f, 0.0f), "-90 degrees yaw"}, {Rotator(0.0f, 0.0f, 90.0f), "90 degrees roll"},
        {Rotator(0.0f, 0.0f, -90.0f), "-90 degrees roll"}, {Rotator(45.0f, 45.0f, 45.0f), "45 degrees all axes"},
        {Rotator(-45.0f, -45.0f, -45.0f), "-45 degrees all axes"}, {Rotator(30.0f, 45.0f, 60.0f), "Combined rotation"},
        {Rotator(-30.0f, -45.0f, -60.0f), "Negative combined rotation"}, {Rotator(89.9f, 0.0f, 0.0f), "Near gimbal lock positive"},
        {Rotator(-89.9f, 0.0f, 0.0f), "Near gimbal lock negative"}, {Rotator(0.0f, 180.0f, 0.0f), "180 degrees yaw"},
        {Rotator(0.0f, -180.0f, 0.0f), "-180 degrees yaw"}, {Rotator(12.35f, 177.0f, 55.546f), "Random"},
        {Rotator(45.0021f, 66.7899f, 0.546f), "Random1"}, {Rotator(0.0021f, 90.7899f, 0.546f), "Random2"}};

    for (const auto& tc : testCases)
    {
        DirectX::XMVECTOR quat = tc.input.ToQuatSIMD();
        Rotator result = Rotator::FromQuat(quat);
        bool isEqual = tc.input.Normalize360() == result.Normalize360();
        if (!isEqual)
        {
            printf("\nTest case: %s\n", tc.description);
            printf("Input  (P,Y,R): %.1f, %.1f, %.1f\n", tc.input.GetPitch(), tc.input.GetYaw(), tc.input.GetRoll());
            printf("Result (P,Y,R): %.1f, %.1f, %.1f\n", result.GetPitch(), result.GetYaw(), result.GetRoll());
        }

        EXPECT_TRUE(isEqual) << "Failed for test case: " << tc.description;
    }
}  

TEST_F(RotatorTest, EulerAnglesConversionTest)
{
    using namespace DirectX;
    // Тестовые случаи
    std::vector<std::tuple<float, float, float>> testCases = {
        {0.0f, 0.0f, 0.0f},        // Нулевые углы
        {45.0f, 90.0f, 30.0f},     // Умеренные углы
        {180.0f, 270.0f, 360.0f},  // Экстремальные углы
        {-30.0f, -45.0f, -60.0f},  // Отрицательные углы
        {90.0f, 0.0f, 0.0f},       // 90 градусов pitch
        {-90.0f, 0.0f, 0.0f},      // -90 градусов pitch
        {0.0f, 90.0f, 0.0f},       // 90 градусов yaw
        {0.0f, -90.0f, 0.0f},      // -90 градусов yaw
        {0.0f, 0.0f, 90.0f},       // 90 градусов roll
        {0.0f, 0.0f, -90.0f},      // -90 градусов roll
        {45.0f, 45.0f, 45.0f},     // 45 градусов по всем осям
        {-45.0f, -45.0f, -45.0f},  // -45 градусов по всем осям
        {30.0f, 45.0f, 60.0f},     // Комбинированные вращения
        {-30.0f, -45.0f, -60.0f},  // Отрицательные комбинированные вращения
        {89.9f, 0.0f, 0.0f},       // Близко к блокировке гимбала положительно
        {-89.9f, 0.0f, 0.0f},      // Близко к блокировке гимбала отрицательно
        {0.0f, 180.0f, 0.0f},      // 180 градусов yaw
        {0.0f, -180.0f, 0.0f}      // -180 градусов yaw
    };

    for (const auto& [pitch, yaw, roll] : testCases)
    {
        std::cout << "\nTest Case: Pitch=" << pitch << ", Yaw=" << yaw << ", Roll=" << roll << std::endl;

        // Установим углы
        rotator.SetData(pitch, yaw, roll);

        // Преобразование напрямую в кватернион
        XMVECTOR directQuat = rotator.ToQuatSIMD();
        directQuat = XMQuaternionNormalize(directQuat);

        // Преобразование через матрицу
        XMMATRIX matrix = rotator.ToMatrix();
        XMVECTOR matrixQuat = Quat::MatrixToQuatSIMD(matrix);
        matrixQuat = XMQuaternionNormalize(matrixQuat);

        // Вывод значений
        PrintQuaternion("Direct Quat", directQuat);
        PrintQuaternion("Matrix Quat", matrixQuat);

        // Сравнение кватернионов с учетом двойственности и погрешности
        bool areEqual = Quat::QuatsIsNearEqual(matrixQuat, directQuat, 1e-3f);
        EXPECT_TRUE(areEqual) << "Quaternions do not match for angles: Pitch=" << pitch << ", Yaw=" << yaw << ", Roll=" << roll
                              << "\nMatrix Quat: " << matrixQuat.m128_f32[0] << " " << matrixQuat.m128_f32[1] << " "
                              << matrixQuat.m128_f32[2] << " " << matrixQuat.m128_f32[3] << "\nDirect Quat: " << directQuat.m128_f32[0]
                              << " " << directQuat.m128_f32[1] << " " << directQuat.m128_f32[2] << " " << directQuat.m128_f32[3];
    }
}

TEST_F(RotatorTest, ExtendedQuaternionEqualityTest)
{
    using namespace DirectX;
    // Дополнительные тестовые случаи
    std::vector<std::tuple<float, float, float>> testCases = {
        {360.0f, 720.0f, 1080.0f}, {-360.0f, -720.0f, -1080.0f}, {15.5f, 33.7f, 77.2f}, {359.99f, 719.99f, 1079.99f}};

    for (const auto& [pitch, yaw, roll] : testCases)
    {
        std::cout << "\nExtended Test Case: Pitch=" << pitch << ", Yaw=" << yaw << ", Roll=" << roll << std::endl;

        rotator.SetData(pitch, yaw, roll);

        XMVECTOR directQuat = rotator.ToQuatSIMD();
        directQuat = XMQuaternionNormalize(directQuat);

        XMMATRIX matrix = rotator.ToMatrix();
        XMVECTOR matrixQuat = Quat::MatrixToQuatSIMD(matrix);
        matrixQuat = XMQuaternionNormalize(matrixQuat);

        PrintQuaternion("Direct Quat", directQuat);
        PrintQuaternion("Matrix Quat", matrixQuat);

        bool areEqual = Quat::QuatsIsNearEqual(matrixQuat, directQuat, 1e-3f);
        EXPECT_TRUE(areEqual) << "Quaternions do not match for extended angles: Pitch=" << pitch << ", Yaw=" << yaw << ", Roll=" << roll
                              << "\nMatrix Quat: " << matrixQuat.m128_f32[0] << " " << matrixQuat.m128_f32[1] << " "
                              << matrixQuat.m128_f32[2] << " " << matrixQuat.m128_f32[3] << "\nDirect Quat: " << directQuat.m128_f32[0]
                              << " " << directQuat.m128_f32[1] << " " << directQuat.m128_f32[2] << " " << directQuat.m128_f32[3];
    }
}

TEST_F(RotatorTest, PrecisionQuaternionEqualityTest)
{
    using namespace DirectX;
    // Тесты с очень малыми изменениями углов
    std::vector<std::tuple<float, float, float>> precisionCases = {
        {0.0001f, 0.0f, 0.0f}, {0.0f, 0.0001f, 0.0f}, {0.0f, 0.0f, 0.0001f}, {0.00001f, 0.00002f, 0.00003f}};

    for (const auto& [pitch, yaw, roll] : precisionCases)
    {
        std::cout << "\nPrecision Test Case: Pitch=" << pitch << ", Yaw=" << yaw << ", Roll=" << roll << std::endl;

        rotator.SetData(pitch, yaw, roll);

        XMVECTOR directQuat = rotator.ToQuatSIMD();
        directQuat = XMQuaternionNormalize(directQuat);

        XMMATRIX matrix = rotator.ToMatrix();
        XMVECTOR matrixQuat = Quat::MatrixToQuatSIMD(matrix);
        matrixQuat = XMQuaternionNormalize(matrixQuat);

        PrintQuaternion("Direct Quat", directQuat);
        PrintQuaternion("Matrix Quat", matrixQuat);

        bool areEqual = Quat::QuatsIsNearEqual(matrixQuat, directQuat, 1e-6f);
        EXPECT_TRUE(areEqual) << "Quaternions do not match for very small angle changes: Pitch=" << pitch << ", Yaw=" << yaw
                              << ", Roll=" << roll << "\nMatrix Quat: " << matrixQuat.m128_f32[0] << " " << matrixQuat.m128_f32[1] << " "
                              << matrixQuat.m128_f32[2] << " " << matrixQuat.m128_f32[3] << "\nDirect Quat: " << directQuat.m128_f32[0]
                              << " " << directQuat.m128_f32[1] << " " << directQuat.m128_f32[2] << " " << directQuat.m128_f32[3];
    }
}  


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
        {0.0f, 0.0f}, {180.0f, 180.0f}, {-180.0f, -180.0f}, {360.0f, 0.0f}, {540.0f, 180.0f}, {-360.0f, 0.0f}, {-540.0f, -180.0f}};

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



// Векторы направления
TEST_F(RotatorTest, DirectionVectors)
{
    Rotator zeroRot;

    auto forward = zeroRot.GetForwardVector();
    auto right = zeroRot.GetRightVector();
    auto up = zeroRot.GetUpVector();

    EXPECT_TRUE(VectorNearEqualWithDebug(forward, Vector(0.0f, 0.0f, 1.0f), zeroRot, "Forward"));
    EXPECT_TRUE(VectorNearEqualWithDebug(right, Vector(1.0f, 0.0f, 0.0f), zeroRot, "Right"));
    EXPECT_TRUE(VectorNearEqualWithDebug(up, Vector(0.0f, 1.0f, 0.0f), zeroRot, "Up"));

    // Проверка с поворотом на 90 градусов вокруг Y
    Rotator yaw90(0.0f, 90.0f, 0.0f);
    auto rotatedForward1 = yaw90.GetForwardVector();
    auto rotatedRight1 = yaw90.GetRightVector();
    auto rotatedUp1 = yaw90.GetUpVector();

    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedForward1, Vector(1.0f, 0.0f, 0.0f), yaw90, "Forward at Yaw 90"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedRight1, Vector(0.0f, 0.0f, -1.0f), yaw90, "Right at Yaw 90"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedUp1, Vector(0.0f, 1.0f, 0.0f), yaw90, "Up at Yaw 90"));

    // Поворот на -90 градусов вокруг Y
    Rotator yawm90(0.0f, -90.0f, 0.0f);
    auto rotatedForward2 = yawm90.GetForwardVector();
    auto rotatedRight2 = yawm90.GetRightVector();
    auto rotatedUp2 = yawm90.GetUpVector();

    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedForward2, Vector(-1.0f, 0.0f, 0.0f), yawm90, "Forward at Yaw -90"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedRight2, Vector(0.0f, 0.0f, 1.0f), yawm90, "Right at Yaw -90"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedUp2, Vector(0.0f, 1.0f, 0.0f), yawm90, "Up at Yaw -90"));

    // Поворот на 180 градусов вокруг Y
    Rotator yaw180(0.0f, 180.0f, 0.0f);
    auto rotatedForward3 = yaw180.GetForwardVector();
    auto rotatedRight3 = yaw180.GetRightVector();
    auto rotatedUp3 = yaw180.GetUpVector();

    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedForward3, Vector(0.0f, 0.0f, -1.0f), yaw180, "Forward at Yaw 180"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedRight3, Vector(-1.0f, 0.0f, 0.0f), yaw180, "Right at Yaw 180"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedUp3, Vector(0.0f, 1.0f, 0.0f), yaw180, "Up at Yaw 180"));

    // Поворот на 90 градусов вокруг X
    Rotator pitch90(90.0f, 0.0f, 0.0f);
    auto rotatedForward4 = pitch90.GetForwardVector();
    auto rotatedRight4 = pitch90.GetRightVector();
    auto rotatedUp4 = pitch90.GetUpVector();

    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedForward4, Vector(0.0f, -1.0f, 0.0f), pitch90, "Forward at Pitch 90"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedRight4, Vector(1.0f, 0.0f, 0.0f), pitch90, "Right at Pitch 90"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedUp4, Vector(0.0f, 0.0f, 1.0f), pitch90, "Up at Pitch 90"));

    // Поворот на -90 градусов вокруг X
    Rotator pitchm90(-90.0f, 0.0f, 0.0f);
    auto rotatedForward5 = pitchm90.GetForwardVector();
    auto rotatedRight5 = pitchm90.GetRightVector();
    auto rotatedUp5 = pitchm90.GetUpVector();

    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedForward5, Vector(0.0f, 1.0f, 0.0f), pitchm90, "Forward at Pitch -90"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedRight5, Vector(1.0f, 0.0f, 0.0f), pitchm90, "Right at Pitch -90"));
    EXPECT_TRUE(VectorNearEqualWithDebug(rotatedUp5, Vector(0.0f, 0.0f, -1.0f), pitchm90, "Up at Pitch -90"));

    // Комбинированный поворот
    Rotator combinedRot(45.0f, 45.0f, 0.0f);
    auto combinedForward = combinedRot.GetForwardVector();
    auto combinedRight = combinedRot.GetRightVector();
    auto combinedUp = combinedRot.GetUpVector();

    // Добавьте здесь проверки для комбинированного поворота с использованием VectorNearEqualWithDebug

    // Дополнительная проверка ортогональности векторов
    std::vector<Rotator> rotations = {zeroRot, yaw90, yawm90, yaw180, pitch90, pitchm90, combinedRot};

    for (const auto& rot : rotations)
    {
        auto f = rot.GetForwardVector();
        auto r = rot.GetRightVector();
        auto u = rot.GetUpVector();

        // Проверка ортогональности (скалярное произведение должно быть близко к 0)
        EXPECT_NEAR(f.Dot(r), 0.0f, 0.001f) << "Forward and Right vectors are not orthogonal for rotation: "
                                            << "Pitch=" << rot.GetPitch() << ", Yaw=" << rot.GetYaw() << ", Roll=" << rot.GetRoll();

        EXPECT_NEAR(f.Dot(u), 0.0f, 0.001f) << "Forward and Up vectors are not orthogonal for rotation: "
                                            << "Pitch=" << rot.GetPitch() << ", Yaw=" << rot.GetYaw() << ", Roll=" << rot.GetRoll();

        EXPECT_NEAR(r.Dot(u), 0.0f, 0.001f) << "Right and Up vectors are not orthogonal for rotation: "
                                            << "Pitch=" << rot.GetPitch() << ", Yaw=" << rot.GetYaw() << ", Roll=" << rot.GetRoll();
    }
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

TEST_F(RotatorTest, Interpolation)
{
    // Начальный и конечный повороты
    Rotator start(0.0f, 0.0f, 0.0f);
    Rotator end(90.0f, 90.0f, 90.0f);

    // Interpolation at 0.0 (start)
    auto result0 = Rotator::Lerp(start, end, 0.0f);
    std::cout << "\nInterpolation at t=0.0:" << std::endl;
    std::cout << "Result Rotator: "
              << "Pitch=" << result0.GetPitch() << ", Yaw=" << result0.GetYaw() << ", Roll=" << result0.GetRoll() << std::endl;
    std::cout << "Expected Start Rotator: "
              << "Pitch=" << start.GetPitch() << ", Yaw=" << start.GetYaw() << ", Roll=" << start.GetRoll() << std::endl;
    EXPECT_TRUE(result0.IsNearEqual(start));

    // Interpolation at 1.0 (end)
    auto result1 = Rotator::Lerp(start, end, 1.0f);
    std::cout << "\nInterpolation at t=1.0:" << std::endl;
    std::cout << "Result Rotator: "
              << "Pitch=" << result1.GetPitch() << ", Yaw=" << result1.GetYaw() << ", Roll=" << result1.GetRoll() << std::endl;
    std::cout << "Expected End Rotator: "
              << "Pitch=" << end.GetPitch() << ", Yaw=" << end.GetYaw() << ", Roll=" << end.GetRoll() << std::endl;
    EXPECT_TRUE(result1.IsNearEqual(end));

    // Interpolation at 0.5 (midpoint)
    auto result05 = Rotator::Lerp(start, end, 0.5f);
    std::cout << "\nInterpolation at t=0.5:" << std::endl;
    std::cout << "Result Rotator: "
              << "Pitch=" << result05.GetPitch() << ", Yaw=" << result05.GetYaw() << ", Roll=" << result05.GetRoll() << std::endl;

    // Проверка средних значений
    std::cout << "Pitch Check: "
              << "Expected=45.0, Actual=" << result05.GetPitch() << ", IsNearEqual=" << IsNearEqual(result05.GetPitch(), 45.0f)
              << std::endl;
    std::cout << "Yaw Check: "
              << "Expected=45.0, Actual=" << result05.GetYaw() << ", IsNearEqual=" << IsNearEqual(result05.GetYaw(), 45.0f) << std::endl;
    std::cout << "Roll Check: "
              << "Expected=45.0, Actual=" << result05.GetRoll() << ", IsNearEqual=" << IsNearEqual(result05.GetRoll(), 45.0f) << std::endl;

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

    std::vector<TestCase> testCases = {{0.0f, 0.0f, 0.0f}, {180.0f, 180.0f, 180.0f}, {360.0f, 0.0f, 0.0f},
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