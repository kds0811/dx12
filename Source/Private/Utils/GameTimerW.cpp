#include "GameTimerW.h"
#include "Windows.h"

GameTimerW::GameTimerW() : SecondsPerCount(0.0), DeltaTime(-1.0), BaseTime(0), PausedTime(0), PrevTime(0), CurrTime(0), Stopped(false)
{
    __int64 countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    SecondsPerCount = 1.0 / (double)countsPerSec;
}

float GameTimerW::GetTotalTime() const
{
    if (Stopped)
    {
        return static_cast<float>(((StopTime - PausedTime) - BaseTime) * SecondsPerCount);
    }
    else
    {
        return static_cast<float>(((CurrTime - PausedTime) - BaseTime) * SecondsPerCount);
    }
}

float GameTimerW::GetDeltaTime() const
{
    return static_cast<float>(DeltaTime);
}

void GameTimerW::Reset()
{
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    CurrTime = currTime;
    BaseTime = currTime;
    PrevTime = currTime;
    StopTime = 0;
    Stopped = false;
    DeltaTime = 0.0;
}

void GameTimerW::Start()
{
    __int64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
    if (Stopped)
    {
        PausedTime += (startTime - StopTime);
        PrevTime = startTime;
        StopTime = 0;
        Stopped = false;
    }
}

void GameTimerW::Stop()
{
    if (!Stopped)
    {
        __int64 currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
        StopTime = currTime;
        Stopped = true;
        DeltaTime = 0.0;
    }
}

void GameTimerW::Tick()
{
    if (Stopped)
    {
        DeltaTime = 0.0;
        return;
    }

    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    CurrTime = currTime;

    DeltaTime = (CurrTime - PrevTime) * SecondsPerCount;

    PrevTime = CurrTime;

    if (DeltaTime < 0.0)
    {
        DeltaTime = 0.0;
    }
}
