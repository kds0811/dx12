#include <gtest/gtest.h>
#include <thread>
#include "GameTimer.h" 

class GameTimerTest : public ::testing::Test
{
protected:
    GameTimer timer;
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
    EXPECT_NEAR(timer.GetTotalTime(), 0.15f, 0.02f);
}
