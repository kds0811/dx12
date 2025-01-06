#include "GameTimerCr.h"

GameTimerCr::GameTimerCr()
{
    HotData.DeltaTime = -1.0;
    HotData.bStopped = false;
    Reset();
}

float GameTimerCr::GetTotalTime() const noexcept
{
    if (HotData.bStopped)
    {
        return static_cast<float>(
                   std::chrono::duration_cast<std::chrono::microseconds>((ColdData.StopTime - ColdData.PausedTime) - ColdData.BaseTime)
                       .count()) *
               static_cast<float>(MultInSecond);
    }
    else
    {
        return static_cast<float>(
                   std::chrono::duration_cast<std::chrono::microseconds>((ColdData.CurrTime - ColdData.PausedTime) - ColdData.BaseTime)
                       .count()) *
               static_cast<float>(MultInSecond);
    }
}

void GameTimerCr::Reset() noexcept
{
    auto currTime = HRC::now();
    ColdData.CurrTime = currTime;
    ColdData.BaseTime = currTime;
    ColdData.PrevTime = currTime;
    ColdData.StopTime = currTime;
    HotData.bStopped = false;
    HotData.DeltaTime = 0.0;
    UpdateCache();
    ColdData.PausedTime = HRC::duration::zero();
}

void GameTimerCr::Start() noexcept
{
    auto startTime = HRC::now();
    if (HotData.bStopped)
    {
        ColdData.PausedTime += std::chrono::duration_cast<std::chrono::microseconds>(startTime - ColdData.StopTime);
        ColdData.PrevTime = startTime;
        ColdData.CurrTime = startTime;
        HotData.bStopped = false;
    }
}

void GameTimerCr::Stop() noexcept
{
    if (!HotData.bStopped)
    {
        auto currTime = HRC::now();
        ColdData.StopTime = currTime;
        HotData.bStopped = true;
        HotData.DeltaTime = 0.0;
    }
}

void GameTimerCr::Tick() noexcept
{
    if (HotData.bStopped)
    {
        HotData.DeltaTime = 0.0;
        return;
    }

    ColdData.CurrTime = HRC::now();

    HotData.DeltaTime = std::chrono::duration<double>(ColdData.CurrTime - ColdData.PrevTime).count();
    HotData.DeltaTime = std::min(HotData.DeltaTime, MaxDeltaTime);
    UpdateCache();

    ColdData.PrevTime = ColdData.CurrTime;
}
