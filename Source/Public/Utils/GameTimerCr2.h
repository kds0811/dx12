#pragma once
#include <chrono>

class GameTimerCr2
{
    using HRC = std::chrono::high_resolution_clock;

public:
    GameTimerCr2();
    [[nodiscard]] inline constexpr float GetDeltaTime() const noexcept { return static_cast<float>(DeltaTime); }  

    float GetTotalTime() const noexcept;
    void Reset() noexcept;
    void Start() noexcept;
    void Stop() noexcept;
    void Tick() noexcept;

private:
    HRC::time_point BaseTime;
    HRC::duration PausedTime;
    HRC::time_point StopTime;
    HRC::time_point PrevTime;
    HRC::time_point CurrTime;
    bool bStopped;
    static constexpr double MaxDeltaTime = 0.25;

    alignas(64) double DeltaTime = 0.0;  // Выравнивание для кэш-линии
    double invMicroToSec = 1.0 / 1000000.0;
};