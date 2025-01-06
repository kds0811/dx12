#include <gtest/gtest.h>
#include <thread>
#include "GameTimerW.h" 
#include "GameTimerCr.h"

class GameTimerTest : public ::testing::Test
{
protected:
    GameTimerCr timer;
};

// ���� ������������� �������
TEST_F(GameTimerTest, Initialization)
{
    timer.Reset();

    EXPECT_LE(timer.GetDeltaTime(), 0.0f);

    EXPECT_NEAR(timer.GetTotalTime(), 0.0f, 0.001f);
}

// ���� �������� ����������� �������
TEST_F(GameTimerTest, BasicTimeProgression)
{
    timer.Reset();
    // �������� Tick ��� �������������
    timer.Tick();

    // ��������� ��������
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ��������� ������
    timer.Tick();

    // DeltaTime ������ ���� ����� 0.1 �������
    EXPECT_NEAR(timer.GetDeltaTime(), 0.1f, 0.01f);

    // ����� ����� ������ ���� ����� 0.1 �������
    EXPECT_NEAR(timer.GetTotalTime(), 0.1f, 0.01f);
}

// ���� �����
TEST_F(GameTimerTest, PauseAndResume)
{
    timer.Reset();
    // �������� ������
    timer.Tick();

    // ��������� ��������
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    float initialTime = timer.GetTotalTime();

    // ������������� ������
    timer.Stop();

    // ���� ��������� �����
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ����� �� ������ ��������
    EXPECT_NEAR(timer.GetTotalTime(), initialTime, 0.001f);

    // ������������
    timer.Start();

    // ���� ��� �������
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    // ����� ����� ������ ���� ����� ���������� + ��������� ����������
    EXPECT_NEAR(timer.GetTotalTime(), initialTime + 0.1f, 0.01f);
}

// ���� ������ �������
TEST_F(GameTimerTest, Reset)
{
    timer.Reset();
    // �������� ������
    timer.Tick();

    // ��������� ��������
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    // ���������� ������
    timer.Reset();

    // ����� ������ ���� ������ � ����
    EXPECT_NEAR(timer.GetTotalTime(), 0.0f, 0.001f);

    // DeltaTime ������ ���� ������ � ����
    EXPECT_NEAR(timer.GetDeltaTime(), 0.0f, 0.001f);
}

// ���� ���������������� �����
TEST_F(GameTimerTest, SequentialTicks)
{
    timer.Reset();

    timer.Tick();  // ������ ���

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.Tick();
    float firstDelta = timer.GetDeltaTime();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();
    float secondDelta = timer.GetDeltaTime();

    // ������ deltaTime ������ ���� ������ �������
    EXPECT_GT(secondDelta, firstDelta);

    // ���������, ��� deltaTime ���������
    EXPECT_GT(secondDelta, 0.0f);
    EXPECT_LT(secondDelta, 0.2f);
}

// ���� ��������� � ������������ �������
TEST_F(GameTimerTest, StopAccuracy)
{
    timer.Reset();

    timer.Tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    float timeBeforeStop = timer.GetTotalTime();

    timer.Stop();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ����� �� ������ ���������� ����� ���������
    EXPECT_NEAR(timer.GetTotalTime(), timeBeforeStop, 0.001f);

    // DeltaTime ������ ���� 0
    EXPECT_NEAR(timer.GetDeltaTime(), 0.0f, 0.001f);
}

// EdgeCase: ������������� �����/����
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

    // ����� ����� ������ ���� ����������
    EXPECT_NEAR(timer.GetTotalTime(), 0.10f, 0.03f);

}

// ���� ������������ ������ �������
TEST_F(GameTimerTest, MaxDeltaTimeLimit)
{
    timer.Reset();

    // �������� �������� ����
    auto startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() < 1000)
    {
        timer.Tick();
    }

    // DeltaTime �� ������ ��������� �������������� ��������� (0.25)
    EXPECT_LE(timer.GetDeltaTime(), 0.25f);
}

// ���� ������� ������� ������
TEST_F(GameTimerTest, HighFrequencyFrames)
{
    timer.Reset();

    // �������� ����� ������ ������
    for (int i = 0; i < 100; ++i)
    {
        timer.Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // ���������, ��� deltaTime � �������� ��������
    EXPECT_GT(timer.GetDeltaTime(), 0.0f);
    EXPECT_LT(timer.GetDeltaTime(), 0.1f);
}

// ���� �������� ��������� ���������� ����������
TEST_F(GameTimerTest, LongRunningAccuracy)
{
    timer.Reset();

    // �������� ����������� ����������
    auto start = std::chrono::steady_clock::now();

    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < 5)
    {
        timer.Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // ����� ����� ������ ���� ������ � 5 ��������
    EXPECT_NEAR(timer.GetTotalTime(), 5.0f, 0.5f);
}

// ���� ���������������� ���������
TEST_F(GameTimerTest, ConsecutiveStops)
{
    timer.Reset();
    timer.Tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    // ������ ���������
    timer.Stop();
    float firstStopTime = timer.GetTotalTime();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // ������ ���������
    timer.Stop();
    float secondStopTime = timer.GetTotalTime();

    // ����� �� ������ ����������
    EXPECT_NEAR(firstStopTime, secondStopTime, 0.001f);
}


// ���� ������������ ����� ���������� �����
TEST_F(GameTimerTest, LongPauseResume)
{
    timer.Reset();
    timer.Tick();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.Tick();

    timer.Stop();

    // ���������� �����
    std::this_thread::sleep_for(std::chrono::seconds(2));

    timer.Start();
    timer.Tick();

    // ����� ����� ����� ������ ���� ����������
    EXPECT_NEAR(timer.GetTotalTime(), 0.1f, 0.05f);
}

// �������������������� ����
TEST_F(GameTimerTest, PerformanceTest)
{
    timer.Reset();

    auto start = std::chrono::steady_clock::now();

    // ������� ���������� �����
    for (int i = 0; i < 10000; ++i)
    {
        timer.Tick();
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // ��� �� ������ �������� ������� ����� �������
    EXPECT_LT(duration.count(), 100);  // ������ 100 �� �� 10000 �����
}