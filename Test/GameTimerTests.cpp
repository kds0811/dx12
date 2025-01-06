#include <gtest/gtest.h>
#include <thread>
#include "GameTimerW.h" 
#include "GameTimerCr.h"

class GameTimerTest : public ::testing::Test
{
protected:
    GameTimerCr timer;
};

// Тест инициализации таймера
TEST_F(GameTimerTest, Initialization)
{
    timer.Reset();

    EXPECT_LE(timer.GetDeltaTime(), 0.0f);

    EXPECT_NEAR(timer.GetTotalTime(), 0.0f, 0.001f);
}

// Тест базового прохождения времени
TEST_F(GameTimerTest, BasicTimeProgression)
{
    timer.Reset();
    // Вызываем Tick для инициализации
    timer.Tick();

    // Небольшая задержка
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Обновляем таймер
    timer.Tick();

    // DeltaTime должен быть около 0.1 секунды
    EXPECT_NEAR(timer.GetDeltaTime(), 0.1f, 0.01f);

    // Общее время должно быть около 0.1 секунды
    EXPECT_NEAR(timer.GetTotalTime(), 0.1f, 0.01f);
}

// Тест паузы
TEST_F(GameTimerTest, PauseAndResume)
{
    timer.Reset();
    // Начинаем отсчет
    timer.Tick();

    // Небольшая задержка
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    float initialTime = timer.GetTotalTime();

    // Останавливаем таймер
    timer.Stop();

    // Ждем некоторое время
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Время не должно меняться
    EXPECT_NEAR(timer.GetTotalTime(), initialTime, 0.001f);

    // Возобновляем
    timer.Start();

    // Ждем еще немного
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    // Общее время должно быть около начального + небольшое приращение
    EXPECT_NEAR(timer.GetTotalTime(), initialTime + 0.1f, 0.01f);
}

// Тест сброса таймера
TEST_F(GameTimerTest, Reset)
{
    timer.Reset();
    // Начинаем отсчет
    timer.Tick();

    // Небольшая задержка
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    // Сбрасываем таймер
    timer.Reset();

    // Время должно быть близко к нулю
    EXPECT_NEAR(timer.GetTotalTime(), 0.0f, 0.001f);

    // DeltaTime должен быть близок к нулю
    EXPECT_NEAR(timer.GetDeltaTime(), 0.0f, 0.001f);
}

// Тест последовательных тиков
TEST_F(GameTimerTest, SequentialTicks)
{
    timer.Reset();

    timer.Tick();  // Первый тик

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.Tick();
    float firstDelta = timer.GetDeltaTime();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();
    float secondDelta = timer.GetDeltaTime();

    // Второй deltaTime должен быть больше первого
    EXPECT_GT(secondDelta, firstDelta);

    // Проверяем, что deltaTime корректен
    EXPECT_GT(secondDelta, 0.0f);
    EXPECT_LT(secondDelta, 0.2f);
}

// Тест остановки и корректности времени
TEST_F(GameTimerTest, StopAccuracy)
{
    timer.Reset();

    timer.Tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    float timeBeforeStop = timer.GetTotalTime();

    timer.Stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Время не должно измениться после остановки
    EXPECT_NEAR(timer.GetTotalTime(), timeBeforeStop, 0.001f);

    // DeltaTime должен быть 0
    EXPECT_NEAR(timer.GetDeltaTime(), 0.0f, 0.001f);
}

// EdgeCase: Множественные старт/стоп
TEST_F(GameTimerTest, MultipleStartStop)
{
    timer.Reset();

    timer.Tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.Tick();

    timer.Stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    timer.Start();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.Tick();

    // Общее время должно быть корректным
    EXPECT_NEAR(timer.GetTotalTime(), 0.10f, 0.03f);

}

// Тест максимальной дельты времени
TEST_F(GameTimerTest, MaxDeltaTimeLimit)
{
    timer.Reset();

    // Эмуляция большого лага
    auto startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() < 1000)
    {
        timer.Tick();
    }

    // DeltaTime не должен превышать установленного максимума (0.25)
    EXPECT_LE(timer.GetDeltaTime(), 0.25f);
}

// Тест высокой частоты кадров
TEST_F(GameTimerTest, HighFrequencyFrames)
{
    timer.Reset();

    // Эмуляция очень частых кадров
    for (int i = 0; i < 100; ++i)
    {
        timer.Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Проверяем, что deltaTime в разумных пределах
    EXPECT_GT(timer.GetDeltaTime(), 0.0f);
    EXPECT_LT(timer.GetDeltaTime(), 0.1f);
}

// Тест точности измерения длительных интервалов
TEST_F(GameTimerTest, LongRunningAccuracy)
{
    timer.Reset();

    // Эмуляция длительного выполнения
    auto start = std::chrono::steady_clock::now();

    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < 5)
    {
        timer.Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Общее время должно быть близко к 5 секундам
    EXPECT_NEAR(timer.GetTotalTime(), 5.0f, 0.5f);
}

// Тест последовательных остановок
TEST_F(GameTimerTest, ConsecutiveStops)
{
    timer.Reset();
    timer.Tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    // Первая остановка
    timer.Stop();
    float firstStopTime = timer.GetTotalTime();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Вторая остановка
    timer.Stop();
    float secondStopTime = timer.GetTotalTime();

    // Время не должно измениться
    EXPECT_NEAR(firstStopTime, secondStopTime, 0.001f);
}


// Тест корректности после длительной паузы
TEST_F(GameTimerTest, LongPauseResume)
{
    timer.Reset();
    timer.Tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    timer.Stop();

    // Длительная пауза
    std::this_thread::sleep_for(std::chrono::seconds(2));

    timer.Start();
    timer.Tick();

    // Время после паузы должно быть корректным
    EXPECT_NEAR(timer.GetTotalTime(), 0.1f, 0.05f);
}

// Производительностный тест
TEST_F(GameTimerTest, PerformanceTest)
{
    timer.Reset();

    auto start = std::chrono::steady_clock::now();

    // Большое количество тиков
    for (int i = 0; i < 10000; ++i)
    {
        timer.Tick();
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Тик не должен занимать слишком много времени
    EXPECT_LT(duration.count(), 100);  // Меньше 100 мс на 10000 тиков
}