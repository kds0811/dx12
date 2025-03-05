#include "KeyboardController.h"
#include "WindowDK.h"

KeyboardController::KeyboardController(Window* wnd) : pWindow(wnd)
{
    unsigned char KeyboardButtons[] = {
        'Q',
        'W',
        'E',
        'R',
        'T',
        'Y',
        'U',
        'I',
        'O',
        'P',
        'A',
        'S',
        'D',
        'F',
        'G',
        'H',
        'J',
        'K',
        'L',
        'Z',
        'X',
        'C',
        'V',
        'B',
        'N',
        'M',
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        '!',
        '@',
        '#',
        '$',
        '%',
        '^',
        '&',
        '*',
        '(',
        ')',
        '-',
        '=',
        '[',
        ']',
        '\\',
        ';',
        '\'',
        ',',
        '.',
        '/',
        '`',
        '~',
        VK_F1,
        VK_F2,
        VK_F3,
        VK_F4,
        VK_F5,
        VK_F6,
        VK_F7,
        VK_F8,
        VK_F9,
        VK_F10,
        VK_F11,
        VK_F12,

        VK_SHIFT,
        VK_CONTROL,
        VK_MENU,
        VK_TAB,
        VK_RETURN,
        VK_SPACE,
        VK_BACK,
        VK_ESCAPE,
        VK_CAPITAL,

        VK_UP,
        VK_DOWN,
        VK_LEFT,
        VK_RIGHT,

        VK_INSERT,
        VK_DELETE,
        VK_HOME,
        VK_END,
        VK_PRIOR,
        VK_NEXT,
    };

    for (const auto& kb : KeyboardButtons)
    {
        mCurrentInput[kb] = false;
    }
}

void KeyboardController::UpdateInput()
{
    for (const auto& [key, value] : mCurrentInput)
    {
        mCurrentInput[key] = pWindow->kbd.KeyIsPressed(key);
    }
}

bool KeyboardController::KeyIsPressed(unsigned char keyCode)
{
    if (mCurrentInput.contains(keyCode))
    {
        return mCurrentInput[keyCode];
    }
    return false;
}
