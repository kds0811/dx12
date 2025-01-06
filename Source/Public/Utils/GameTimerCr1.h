#pragma once
#include <chrono>

class GameTimerCr1
{
    using HRC = std::chrono::high_resolution_clock;

public:
    GameTimerCr1();
    [[nodiscard]] inline constexpr float GetDeltaTime() const noexcept
    {
        if (isDeltaCached)
        {
            return cachedDeltaTime;
        }
        return static_cast<float>(DeltaTime);
    }  

    float GetTotalTime() const noexcept;
    void Reset() noexcept;
    void Start() noexcept;
    void Stop() noexcept;
    void Tick() noexcept;

private:
    double DeltaTime;
    HRC::time_point BaseTime;
    HRC::duration PausedTime;
    HRC::time_point StopTime;
    HRC::time_point PrevTime;
    HRC::time_point CurrTime;
    bool bStopped;
    static constexpr double MaxDeltaTime = 0.25;

    mutable float cachedDeltaTime = 0.0f;
    bool isDeltaCached = false;
};