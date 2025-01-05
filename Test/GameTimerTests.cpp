#include <gtest/gtest.h>
#include <thread>
#include "GameTimer.h" 

class GameTimerTest : public ::testing::Test
{
protected:
    GameTimer timer;
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
    EXPECT_NEAR(timer.GetTotalTime(), 0.15f, 0.02f);
}
