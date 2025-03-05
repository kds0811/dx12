#pragma once
#include <memory>
#include "MouseController.h"
#include "KeyboardController.h"
#include "DirectXMath.h"

class Window;

class MainInputController
{
    std::unique_ptr<MouseController> mMouseController = nullptr;
    std::unique_ptr<KeyboardController> mKeyboardController = nullptr;

public:
    MainInputController(Window* wnd);
    void Update();
    bool KeyboardKeyIsPressed(unsigned char keyCode);
    bool MouseButtonIsPressed(unsigned char keyCode);
    DirectX::XMFLOAT2 GetMousePositionOffset();

};