#pragma once
#include <unordered_map>

class Window;

class KeyboardController
{
    Window* pWindow = nullptr;
    std::unordered_map<unsigned char, bool> mCurrentInput;

public:
    KeyboardController(Window* wnd);

    void UpdateInput();

    bool KeyIsPressed(unsigned char keyCode);

};