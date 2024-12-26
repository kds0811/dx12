#pragma once
#include <queue>

class Mouse
{
    friend class Window;

public:
    class EventM
    {
    public:
        enum class Type
        {
            LPress,
            LRelease,
            RPress,
            RRelease,
            WheelUp,
            WheelDown,
            Move,
            Enter,
            Leave,
            Invalid
        };

    private:
        Type type = Type::Invalid;
        int x = 0;
        int y = 0;
        bool lPressed = false;
        bool rPressed = false;

    public:
        EventM() : type(Type::Invalid), x(0), y(0), lPressed(false), rPressed(false) {}
        EventM(Type type, Mouse parent) : type(type), x(parent.x), y(parent.y), lPressed(parent.lPressed), rPressed(parent.rPressed) {}
        Type GetType() { return type; }
        // Vec2 GetPosVec()
        int GetPosX() { return x; }
        int GetPosY() { return y; }
        bool LIsPressed() { return lPressed; }
        bool RIsPressed() { return rPressed; }
    };

public:
    Mouse();
    Mouse& operator=(const Mouse&) = delete;
    // Vec2 GetPosVec();
    int GetPosX();
    int GetPosY();
    bool IsInWindow();
    bool LIsPressed();
    bool RIsPressed();
    Mouse::EventM Read();
    bool IsEmpty();
    void Flush();

private:
    void OnMouseMove(int x_in, int y_in);
    void OnLeftIsPressed();
    void OnLeftIsReleased();
    void OnRightIsPressed();
    void OnRightIsReleased();
    void OnwheelUP();
    void OnWheelDown();
    void OnMouseLeave();
    void OnMouseEnter();
    void OnWheelDeta(int delta);
    void TrimBuffer();

private:
    static constexpr unsigned int bufferSize = 16u;
    int x = 0;
    int y = 0;
    bool lPressed = false;
    bool rPressed = false;
    bool isInWindow = false;
    int wheelDeltaCarry = 0;
    std::queue<EventM> buffer;
};
