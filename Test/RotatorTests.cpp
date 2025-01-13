#include <gtest/gtest.h>
#include <DirectXMath.h>
#include "Rotator.h"
#include <iostream>
#include <iomanip>
#include "Vector.h"

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
        {Rotator(0.0f, -180.0f, 0.0f), "-180 degrees yaw"}};

    for (const auto& tc : testCases)
    {
        DirectX::XMVECTOR quat = tc.input.ToQuaternion();
        Rotator result = Rotator::FromQuaternion(quat);
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
        rotator.Data = {pitch, yaw, roll};

        // Преобразование напрямую в кватернион
        XMVECTOR directQuat = rotator.ToQuaternion();
        directQuat = XMQuaternionNormalize(directQuat);

        // Преобразование через матрицу
        XMMATRIX matrix = rotator.ToMatrix();
        XMVECTOR matrixQuat = rotator.MatrixToQuat(matrix);
        matrixQuat = XMQuaternionNormalize(matrixQuat);

        // Вывод значений
        PrintQuaternion("Direct Quat", directQuat);
        PrintQuaternion("Matrix Quat", matrixQuat);

        // Сравнение кватернионов с учетом двойственности и погрешности
        bool areEqual = Rotator::QuaternionsAreEqual(matrixQuat, directQuat, 1e-3f);
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

        rotator.Data = {pitch, yaw, roll};

        XMVECTOR directQuat = rotator.ToQuaternion();
        directQuat = XMQuaternionNormalize(directQuat);

        XMMATRIX matrix = rotator.ToMatrix();
        XMVECTOR matrixQuat = rotator.MatrixToQuat(matrix);
        matrixQuat = XMQuaternionNormalize(matrixQuat);

        PrintQuaternion("Direct Quat", directQuat);
        PrintQuaternion("Matrix Quat", matrixQuat);

        bool areEqual = Rotator::QuaternionsAreEqual(matrixQuat, directQuat, 1e-3f);
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

        rotator.Data = {pitch, yaw, roll};

        XMVECTOR directQuat = rotator.ToQuaternion();
        directQuat = XMQuaternionNormalize(directQuat);

        XMMATRIX matrix = rotator.ToMatrix();
        XMVECTOR matrixQuat = rotator.MatrixToQuat(matrix);
        matrixQuat = XMQuaternionNormalize(matrixQuat);

        PrintQuaternion("Direct Quat", directQuat);
        PrintQuaternion("Matrix Quat", matrixQuat);

        bool areEqual = Rotator::QuaternionsAreEqual(matrixQuat, directQuat, 1e-6f);
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
    EXPECT_TRUE(VectorNearEqual(rotatedForward, Vector(1.0f, 0.0f, 0.0f)));
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

    // Вывод информации о начальном и конечном поворотах
    std::cout << "Interpolation Test:" << std::endl;
    std::cout << "Start Rotator: "
              << "Pitch=" << start.GetPitch() << ", Yaw=" << start.GetYaw() << ", Roll=" << start.GetRoll() << std::endl;
    std::cout << "End Rotator: "
              << "Pitch=" << end.GetPitch() << ", Yaw=" << end.GetYaw() << ", Roll=" << end.GetRoll() << std::endl;

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