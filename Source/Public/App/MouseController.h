#pragma once
#include <unordered_map>
#include <DirectXMath.h>

class Window;

class MouseController
{
    Window* pWindow;
    std::unordered_map<unsigned char, bool> mCurrentInput;
    DirectX::XMFLOAT2 mPrevMousePosition = {0.0f, 0.0f};
    DirectX::XMFLOAT2 mMouseOffset = {0.0f, 0.0f};

public:
    MouseController(Window* wnd);
    void UpdateInput();
    bool ButtonIsPressed(unsigned char KeyCode);
    inline DirectX::XMFLOAT2 GetMousePosOffset() const { return mMouseOffset; }

};