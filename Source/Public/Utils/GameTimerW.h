#pragma once

class GameTimerW
{
public:
    GameTimerW();
    float GetTotalTime() const; 

    float GetDeltaTime() const; 
    void Reset();             // Call before message loop.
    void Start();             // Call when unpaused.
    void Stop();              // Call when paused.
    void Tick();              // Call every frame.
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