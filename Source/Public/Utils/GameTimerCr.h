#pragma once
#include <chrono>

class GameTimerCr
{
    using HRC = std::chrono::high_resolution_clock;


public:
    GameTimerCr();
    float GetTotalTime() const noexcept;   // in seconds
    float GetDeltaTime() const noexcept;  // in seconds
    void Reset() noexcept;                 // Call before message loop.
    void Start() noexcept;                // Call when unpaused.
    void Stop() noexcept;                  // Call when paused.
    void Tick() noexcept;                 // Call every frame.
private:
    double DeltaTime;
    HRC::time_point BaseTime;
    HRC::duration PausedTime;
    HRC::time_point StopTime;
    HRC::time_point PrevTime;
    HRC::time_point CurrTime;
    bool bStopped;
    static constexpr double MaxDeltaTime = 0.25;

};