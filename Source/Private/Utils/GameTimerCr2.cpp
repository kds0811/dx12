#include "GameTimerCr2.h"

GameTimerCr2::GameTimerCr2() :  DeltaTime(-1.0), bStopped(false)
{
    Reset();
}

float GameTimerCr2::GetTotalTime() const noexcept
{
    if (bStopped)
    {
        return static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>((StopTime - PausedTime) - BaseTime).count()) /
               1000000.0f;
    }
    else
    {
        return static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>((CurrTime - PausedTime) - BaseTime).count()) /
               1000000.0f;
    }
}

void GameTimerCr2::Reset() noexcept
{
    auto currTime = HRC::now();
    CurrTime = currTime;
    BaseTime = currTime;
    PrevTime = currTime;
    StopTime = currTime;
    bStopped = false;
    DeltaTime = 0.0;
    PausedTime = HRC::duration::zero();
}

void GameTimerCr2::Start() noexcept
{
    auto startTime = HRC::now();
    if (bStopped)
    {
        PausedTime += std::chrono::duration_cast<std::chrono::microseconds>(startTime - StopTime);
        PrevTime = startTime;
        CurrTime = startTime;
        bStopped = false;
    }
}

void GameTimerCr2::Stop() noexcept
{
    if (!bStopped)
    {
        auto currTime = HRC::now();
        StopTime = currTime;
        bStopped = true;
        DeltaTime = 0.0;
    }
}

void GameTimerCr2::Tick() noexcept
{
    if (bStopped)
    {
        DeltaTime = 0.0;
        return;
    }

    auto currTime = HRC::now();

    // Прямое вычисление без промежуточных объектов
    auto microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(currTime - PrevTime).count();

    // Быстрое умножение вместо деления
    DeltaTime = microSeconds * invMicroToSec;

    // Простое ограничение с константой
    constexpr double MAX_DELTA = 0.25;
    DeltaTime = (DeltaTime > MAX_DELTA) ? MAX_DELTA : DeltaTime;

    PrevTime = currTime;
}
