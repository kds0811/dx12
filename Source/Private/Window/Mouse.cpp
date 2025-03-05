#include "Window/Mouse.h"
#include "Window/WindowDK.h"

Mouse::Mouse() : x(0), y(0) {}

DirectX::XMFLOAT2 Mouse::GetPosVec()
{
    return DirectX::XMFLOAT2{static_cast<float>(x), static_cast<float>(y)};
 }

int Mouse::GetPosX() const
{
    return x;
}

int Mouse::GetPosY() const
{
    return y;
}

bool Mouse::IsInWindow() const
{
    return isInWindow;
}

bool Mouse::LIsPressed() const
{
    return bLPressed;
}

bool Mouse::RIsPressed() const
{
    return bRPressed;
}

bool Mouse::MIsPressed() const
{
    return bMPressed;
}

Mouse::EventM Mouse::Read()
{
    if (buffer.size() > 0)
    {
        Mouse::EventM e = buffer.front();
        buffer.pop();
        return e;
    }
    else
    {
        return Mouse::EventM();
    }
}

bool Mouse::IsEmpty()
{
    return buffer.empty();
}

void Mouse::Flush()
{
    buffer = std::queue<EventM>{};
}

void Mouse::OnMouseMove(int x_in, int y_in)
{
    x = x_in;
    y = y_in;
    buffer.push(Mouse::EventM(EventM::Type::Move, *this));
    TrimBuffer();
}

void Mouse::OnLeftIsPressed()
{
    bLPressed = true;
    buffer.push(Mouse::EventM(EventM::Type::LPress, *this));
    TrimBuffer();
}

void Mouse::OnLeftIsReleased()
{
    bLPressed = false;
    buffer.push(Mouse::EventM(EventM::Type::LRelease, *this));
    TrimBuffer();
}

void Mouse::OnRightIsPressed()
{
    bRPressed = true;
    buffer.push(Mouse::EventM(EventM::Type::RPress, *this));
    TrimBuffer();
}

void Mouse::OnRightIsReleased()
{
    bRPressed = false;
    buffer.push(Mouse::EventM(EventM::Type::RRelease, *this));
    TrimBuffer();
}

void Mouse::OnWheelIsPressed() 
{
    bMPressed = true;
    buffer.push(Mouse::EventM(EventM::Type::MPress, *this));
    TrimBuffer();
}

void Mouse::OnWheelIsReleased()
{
    bMPressed = false;
    buffer.push(Mouse::EventM(EventM::Type::MRelease, *this));
    TrimBuffer();
}

void Mouse::OnwheelUP()
{
    buffer.push(Mouse::EventM(EventM::Type::WheelUp, *this));
    TrimBuffer();
}

void Mouse::OnWheelDown()
{
    buffer.push(Mouse::EventM(EventM::Type::WheelDown, *this));
    TrimBuffer();
}

void Mouse::OnMouseLeave()
{
    isInWindow = false;
    buffer.push(Mouse::EventM(EventM::Type::Leave, *this));
    TrimBuffer();
}

void Mouse::OnMouseEnter()
{
    isInWindow = true;
    buffer.push(Mouse::EventM(EventM::Type::Enter, *this));
    TrimBuffer();
}

void Mouse::OnWheelDeta(int delta)
{
    wheelDeltaCarry += delta;
    while (wheelDeltaCarry >= WHEEL_DELTA)
    {
        wheelDeltaCarry -= WHEEL_DELTA;
        OnwheelUP();
    }
    while (wheelDeltaCarry <= -WHEEL_DELTA)
    {
        wheelDeltaCarry += WHEEL_DELTA;
        OnWheelDown();
    }
}

void Mouse::TrimBuffer()
{
    while (buffer.size() > bufferSize)
    {
        buffer.pop();
    }
}
