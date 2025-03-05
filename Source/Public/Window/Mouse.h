#pragma once
#include <queue>
#include <DirectXMath.h>

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
            Invalid,
            MPress,
            MRelease
        };

    private:
        Type type = Type::Invalid;
        int x = 0;
        int y = 0;
        bool lPressed = false;
        bool rPressed = false;

    public:
        EventM() : type(Type::Invalid), x(0), y(0), lPressed(false), rPressed(false) {}
        EventM(Type type, Mouse parent) : type(type), x(parent.x), y(parent.y), lPressed(parent.bLPressed), rPressed(parent.bRPressed) {}
        Type GetType() const { return type; }
        DirectX::XMFLOAT2 GetPosVec() const { return DirectX::XMFLOAT2{static_cast<float>(x), static_cast<float>(y)}; };
        int GetPosX() const { return x; }
        int GetPosY() const { return y; }
        bool LIsPressed() const { return lPressed; }
        bool RIsPressed() const { return rPressed; }
    };

public:
    Mouse();
    Mouse& operator=(const Mouse&) = delete;
    DirectX::XMFLOAT2 GetPosVec();
    int GetPosX() const;
    int GetPosY() const;
    bool IsInWindow() const;
    bool LIsPressed() const;
    bool RIsPressed() const;
    bool MIsPressed() const;
    Mouse::EventM Read();
    bool IsEmpty();
    void Flush();

private:
    void OnMouseMove(int x_in, int y_in);
    void OnLeftIsPressed();
    void OnLeftIsReleased();
    void OnRightIsPressed();
    void OnRightIsReleased();

    void OnWheelIsPressed();
    void OnWheelIsReleased();

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
    bool bLPressed = false;
    bool bRPressed = false;
    bool bMPressed = false;

    bool isInWindow = false;
    int wheelDeltaCarry = 0;
    std::queue<EventM> buffer;
};
