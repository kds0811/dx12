#include "GameTimerCr.h"

GameTimerCr::GameTimerCr() :  DeltaTime(-1.0), bStopped(false)
{
    Reset();
}

float GameTimerCr::GetTotalTime() const noexcept
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

float GameTimerCr::GetDeltaTime() const noexcept
{
    return static_cast<float>(DeltaTime);
}

void GameTimerCr::Reset() noexcept
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

void GameTimerCr::Start() noexcept
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

void GameTimerCr::Stop() noexcept
{
    if (!bStopped)
    {
        auto currTime = HRC::now();
        StopTime = currTime;
        bStopped = true;
        DeltaTime = 0.0;
    }
}

void GameTimerCr::Tick() noexcept
{
    if (bStopped)
    {
        DeltaTime = 0.0;
        return;
    }

    auto currTime = HRC::now();
    CurrTime = currTime;

    DeltaTime = std::chrono::duration<double>(CurrTime - PrevTime).count();

    PrevTime = CurrTime;

    DeltaTime = std::min(DeltaTime, MaxDeltaTime);
}
