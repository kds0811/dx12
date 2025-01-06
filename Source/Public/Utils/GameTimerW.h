#pragma once

class GameTimerW
{
public:
    GameTimerW();
    [[nodiscard]] inline constexpr float GetDeltaTime() const noexcept { return static_cast<float>(DeltaTime); }
    float GetTotalTime() const noexcept;
    void Reset() noexcept;
    void Start() noexcept;
    void Stop() noexcept;
    void Tick() noexcept;

private:
    double SecondsPerCount;
    double DeltaTime;
    __int64 BaseTime;
    __int64 PausedTime;
    __int64 StopTime;
    __int64 PrevTime;
    __int64 CurrTime;
    bool Stopped;
};