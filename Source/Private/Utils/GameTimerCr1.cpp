#include "GameTimerCr1.h"

GameTimerCr1::GameTimerCr1() :  DeltaTime(-1.0), bStopped(false)
{
    Reset();
}

float GameTimerCr1::GetTotalTime() const noexcept
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

void GameTimerCr1::Reset() noexcept
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

void GameTimerCr1::Start() noexcept
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

void GameTimerCr1::Stop() noexcept
{
    if (!bStopped)
    {
        auto currTime = HRC::now();
        StopTime = currTime;
        bStopped = true;
        DeltaTime = 0.0;
    }
}

void GameTimerCr1::Tick() noexcept
{
    if (bStopped)
    {
        cachedDeltaTime = 0.0f;
        isDeltaCached = true;
        DeltaTime = 0.0;
        return;
    }

    auto currTime = HRC::now();

    // Используем напрямую duration_cast для снижения overhead
    auto deltaDuration = std::chrono::duration_cast<std::chrono::microseconds>(currTime - PrevTime);

    // Быстрое преобразование без создания промежуточных объектов
    DeltaTime = deltaDuration.count() / 1000000.0;

    // Кэшируем для быстрого доступа
    cachedDeltaTime = static_cast<float>(DeltaTime);
    isDeltaCached = true;

    // Ограничение максимальной дельты
    constexpr double MAX_DELTA = 0.25;
    DeltaTime = std::min(DeltaTime, MAX_DELTA);

    PrevTime = currTime;
}
