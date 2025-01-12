#include <gtest/gtest.h>
#include <cmath>
#include "Vector.h"

   class VectorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Базовые векторы для тестов
        v1 = Vector(1.0f, 2.0f, 3.0f);
        v2 = Vector(4.0f, 5.0f, 6.0f);
        zero = Vector::Zero();
        one = Vector::One();
    }

    const float epsilon = 1e-4f;
    Vector v1, v2, zero, one;
};

// Тесты конструкторов
TEST_F(VectorTest, Constructors)
{
    // Дефолтный конструктор
    Vector v;
    EXPECT_FLOAT_EQ(v.Data.x, 0.0f);
    EXPECT_FLOAT_EQ(v.Data.y, 0.0f);
    EXPECT_FLOAT_EQ(v.Data.z, 0.0f);

    // Конструктор с параметрами
    Vector v3(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(v3.Data.x, 1.0f);
    EXPECT_FLOAT_EQ(v3.Data.y, 2.0f);
    EXPECT_FLOAT_EQ(v3.Data.z, 3.0f);

    // Конструктор из XMFLOAT3
    DirectX::XMFLOAT3 xmf3(1.0f, 2.0f, 3.0f);
    Vector v4(xmf3);
    EXPECT_FLOAT_EQ(v4.Data.x, 1.0f);
    EXPECT_FLOAT_EQ(v4.Data.y, 2.0f);
    EXPECT_FLOAT_EQ(v4.Data.z, 3.0f);

    // Конструктор из FXMVECTOR
    DirectX::XMVECTOR xmv = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
    Vector v5(xmv);
    EXPECT_FLOAT_EQ(v5.Data.x, 1.0f);
    EXPECT_FLOAT_EQ(v5.Data.y, 2.0f);
    EXPECT_FLOAT_EQ(v5.Data.z, 3.0f);
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

// Тесты операторов сложения
TEST_F(VectorTest, Addition)
{
    Vector sum = v1 + v2;
    EXPECT_FLOAT_EQ(sum.Data.x, 5.0f);
    EXPECT_FLOAT_EQ(sum.Data.y, 7.0f);
    EXPECT_FLOAT_EQ(sum.Data.z, 9.0f);

    Vector v3 = v1;
    v3 += v2;
    EXPECT_FLOAT_EQ(v3.Data.x, 5.0f);
    EXPECT_FLOAT_EQ(v3.Data.y, 7.0f);
    EXPECT_FLOAT_EQ(v3.Data.z, 9.0f);
}

// Тесты операторов вычитания
TEST_F(VectorTest, Subtraction)
{
    Vector diff = v2 - v1;
    EXPECT_FLOAT_EQ(diff.Data.x, 3.0f);
    EXPECT_FLOAT_EQ(diff.Data.y, 3.0f);
    EXPECT_FLOAT_EQ(diff.Data.z, 3.0f);

    Vector v3 = v2;
    v3 -= v1;
    EXPECT_FLOAT_EQ(v3.Data.x, 3.0f);
    EXPECT_FLOAT_EQ(v3.Data.y, 3.0f);
    EXPECT_FLOAT_EQ(v3.Data.z, 3.0f);
}

// Тесты операторов умножения
TEST_F(VectorTest, Multiplication)
{
    float scalar = 2.0f;
    Vector prod = v1 * scalar;
    EXPECT_FLOAT_EQ(prod.Data.x, 2.0f);
    EXPECT_FLOAT_EQ(prod.Data.y, 4.0f);
    EXPECT_FLOAT_EQ(prod.Data.z, 6.0f);

    Vector v3 = v1;
    v3 *= scalar;
    EXPECT_FLOAT_EQ(v3.Data.x, 2.0f);
    EXPECT_FLOAT_EQ(v3.Data.y, 4.0f);
    EXPECT_FLOAT_EQ(v3.Data.z, 6.0f);
}

// Тесты операторов деления
TEST_F(VectorTest, Division)
{
    float scalar = 2.0f;
    Vector quot = v1 / scalar;
    EXPECT_FLOAT_EQ(quot.Data.x, 0.5f);
    EXPECT_FLOAT_EQ(quot.Data.y, 1.0f);
    EXPECT_FLOAT_EQ(quot.Data.z, 1.5f);

    Vector v3 = v1;
    v3 /= scalar;
    EXPECT_FLOAT_EQ(v3.Data.x, 0.5f);
    EXPECT_FLOAT_EQ(v3.Data.y, 1.0f);
    EXPECT_FLOAT_EQ(v3.Data.z, 1.5f);
}

// Тесты операторов сравнения
TEST_F(VectorTest, Comparison)
{
    Vector v3(1.0f, 2.0f, 3.0f);
    EXPECT_TRUE(v1 == v3);
    EXPECT_FALSE(v1 != v3);
    EXPECT_TRUE(v1 != v2);
}

// Тест NearEqual
TEST_F(VectorTest, NearEqual)
{
    Vector v3(1.001f, 2.001f, 3.001f);
    EXPECT_TRUE(v1.NearEqual(v3, 0.01f));
    EXPECT_FALSE(v1.NearEqual(v3, 0.0001f));
}

// Тесты математических функций
TEST_F(VectorTest, MathFunctions)
{
    // Abs
    Vector neg(-1.0f, -2.0f, -3.0f);
    Vector abs = neg.Abs();
    EXPECT_FLOAT_EQ(abs.Data.x, 1.0f);
    EXPECT_FLOAT_EQ(abs.Data.y, 2.0f);
    EXPECT_FLOAT_EQ(abs.Data.z, 3.0f);

    // Length
    EXPECT_NEAR(v1.Length(), std::sqrt(14.0f), epsilon);
    EXPECT_NEAR(v1.LengthSq(), 14.0f, epsilon);
    EXPECT_NEAR(v1.LengthEst(), std::sqrt(14.0f), 0.1f);

    // Normalize
    Vector norm = v1.Normalize();
    EXPECT_NEAR(norm.Length(), 1.0f, epsilon);

    // Dot Product
    float dot = v1.Dot(v2);
    EXPECT_NEAR(dot, 32.0f, epsilon);

    // Cross Product
    Vector cross = v1.Cross(v2);
    EXPECT_NEAR(cross.Data.x, -3.0f, epsilon);
    EXPECT_NEAR(cross.Data.y, 6.0f, epsilon);
    EXPECT_NEAR(cross.Data.z, -3.0f, epsilon);

    // Angle
    Vector x(1.0f, 0.0f, 0.0f);
    Vector y(0.0f, 1.0f, 0.0f);
    EXPECT_NEAR(x.Angle(y), DirectX::XM_PIDIV2, epsilon);

    // Clamp
    Vector min(-1.0f, -1.0f, -1.0f);
    Vector max(2.0f, 2.0f, 2.0f);
    Vector clamped = v1.Clamp(min, max);
    EXPECT_TRUE(clamped == Vector(1.0f, 2.0f, 2.0f));
}

// Тесты статических функций
TEST_F(VectorTest, StaticFunctions)
{
    // Zero, One, Up
    EXPECT_TRUE(Vector::Zero() == Vector(0.0f, 0.0f, 0.0f));
    EXPECT_TRUE(Vector::One() == Vector(1.0f, 1.0f, 1.0f));
    EXPECT_TRUE(Vector::Up() == Vector(0.0f, 1.0f, 0.0f));

    // Distance
    float dist = Vector::Distance(v1, v2);
    EXPECT_NEAR(dist, std::sqrt(27.0f), epsilon);

    // Lerp
    Vector start(0.0f, 0.0f, 0.0f);
    Vector end(2.0f, 2.0f, 2.0f);
    Vector mid = Vector::Lerp(start, end, 0.5f);
    EXPECT_TRUE(mid.NearEqual(Vector(1.0f, 1.0f, 1.0f)));
}

// Тест конвертации SIMD
TEST_F(VectorTest, SIMDConversion)
{
    DirectX::XMVECTOR simd = v1.ToSIMD();
    Vector converted(simd);
    EXPECT_TRUE(converted == v1);
}

// Тест граничных случаев
TEST_F(VectorTest, EdgeCases)
{
    // Деление на ноль
    Vector v3 = v1 / 0.0f;
    EXPECT_TRUE(std::isinf(v3.Data.x) || std::isnan(v3.Data.x));

    // Нормализация нулевого вектора
    Vector normalizedZero = zero.Normalize();
    EXPECT_TRUE(normalizedZero == Vector::Zero());

}

// Тест производительности (опционально)
TEST_F(VectorTest, Performance)
{
    const int iterations = 100000;
    Vector result;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i)
    {
        result = v1 + v2;
        result = result * 2.0f;
        result = result.Normalize();
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Performance test duration: " << duration.count() << " microseconds" << std::endl;

    EXPECT_NE(result, Vector::Zero());  // Предотвращаем оптимизацию
}


// Тест конструкторов копирования и перемещения
TEST_F(VectorTest, CopyAndMoveConstructors)
{
    // Конструктор копирования
    Vector v3(v1);
    EXPECT_TRUE(v1 == v3);

    // Конструктор перемещения
    Vector v4 = std::move(v3);
    EXPECT_TRUE(v1 == v4);
    EXPECT_TRUE(v3 == Vector::Zero());  // Перемещенный объект обнуляется
}

// Тест операторов присваивания
TEST_F(VectorTest, AssignmentOperators)
{
    // Копирующее присваивание
    Vector v3;
    v3 = v1;
    EXPECT_TRUE(v1 == v3);

    // Перемещающее присваивание
    Vector v4;
    v4 = std::move(v3);
    EXPECT_TRUE(v1 == v4);
    EXPECT_TRUE(v3 == Vector::Zero());  // Перемещенный объект обнуляется
}

// Тест поворота вектора
TEST_F(VectorTest, Rotation)
{
    // Создаем кватернион поворота вокруг оси Y на 90 градусов
    DirectX::XMVECTOR rotationQuaternion =
        DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XM_PIDIV2);

    Vector originalVector(1.0f, 0.0f, 0.0f);

    // Прямой поворот
    Vector rotatedVector = originalVector.Rotate(rotationQuaternion);
    EXPECT_NEAR(rotatedVector.GetX(), 0.0f, epsilon);
    EXPECT_NEAR(rotatedVector.GetY(), 0.0f, epsilon);
    EXPECT_NEAR(rotatedVector.GetZ(), -1.0f, epsilon);

    // Обратный поворот
    Vector inverseRotatedVector = originalVector.InverseRotate(rotationQuaternion);
    EXPECT_NEAR(inverseRotatedVector.GetX(), 0.0f, epsilon);
    EXPECT_NEAR(inverseRotatedVector.GetY(), 0.0f, epsilon);
    EXPECT_NEAR(inverseRotatedVector.GetZ(), 1.0f, epsilon);
}

// Расширенные тесты для Dot и Cross продукта
TEST_F(VectorTest, DotAndCrossProduct)
{
    // Ортогональные векторы
    Vector x(1.0f, 0.0f, 0.0f);
    Vector y(0.0f, 1.0f, 0.0f);
    Vector z(0.0f, 0.0f, 1.0f);

    // Dot product
    EXPECT_FLOAT_EQ(x.Dot(y), 0.0f);
    EXPECT_FLOAT_EQ(x.Dot(x), 1.0f);

    // Cross product
    Vector crossXY = x.Cross(y);
    EXPECT_TRUE(crossXY == z);

    Vector crossYX = y.Cross(x);
    EXPECT_TRUE(crossXY == crossYX * -1.0f);
}


// Тест на инварианты операций
TEST_F(VectorTest, Invariants)
{
    // a + (-a) должно быть Zero()
    Vector negV1 = v1 * -1.0f;
    EXPECT_TRUE((v1 + negV1).NearEqual(Vector::Zero()));

    // |a * k| = |a| * |k|
    float scalar = 2.0f;
    EXPECT_NEAR((v1 * scalar).Length(), v1.Length() * std::abs(scalar), epsilon);

    // Дистанция от вектора до себя = 0
    EXPECT_NEAR(Vector::Distance(v1, v1), 0.0f, epsilon);
}

// Тест совместимости с DirectX Math
TEST_F(VectorTest, DirectXMathCompatibility)
{
    // Загрузка из SIMD вектора
    DirectX::XMVECTOR simdVector = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
    Vector fromSIMD(simdVector);

    // Преобразование обратно в SIMD
    DirectX::XMVECTOR backToSIMD = fromSIMD.ToSIMD();

    // Проверка компонент
    float components[4];
    DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(components), backToSIMD);

    EXPECT_NEAR(components[0], 1.0f, epsilon);
    EXPECT_NEAR(components[1], 2.0f, epsilon);
    EXPECT_NEAR(components[2], 3.0f, epsilon);
}