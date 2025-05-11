#pragma once
#include <chrono>

class GameTimerCr
{
    using HRC = std::chrono::high_resolution_clock;

public:
    GameTimerCr();
    [[nodiscard]] inline constexpr float GetDeltaTime() const noexcept { return HotData.CachedDeltaTime; }

    float GetTotalTime() const noexcept;
    void Reset() noexcept;
    void Start() noexcept;
    void Stop() noexcept;
    void Tick() noexcept;

private:
    alignas(64) struct
    {  
        float CachedDeltaTime;
        double DeltaTime;
        bool bStopped;
    } HotData; 

     struct
    {
        HRC::time_point BaseTime;
        HRC::duration PausedTime;
        HRC::time_point StopTime;
        HRC::time_point PrevTime;
        HRC::time_point CurrTime;
    } ColdData;

    static constexpr double MaxDeltaTime = 0.25;
    static constexpr double MultInSecond = 1.0 / 1000000.0;

    [[nodiscard]] inline constexpr void UpdateCache() noexcept { HotData.CachedDeltaTime = static_cast<float>(HotData.DeltaTime); }
};