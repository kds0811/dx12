#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode)
{
    return keyStates[keycode];
}

Keyboard::EventK Keyboard::ReadKey()
{
    if (keyBuffer.size() > 0)
    {
        Keyboard::EventK e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }
    else
    {
        return Keyboard::EventK();
    }
}

bool Keyboard::KeyIsEmpty()
{
    return keyBuffer.empty();
}

void Keyboard::FlushKey()
{
    keyBuffer = std::queue<EventK>{};
}

char Keyboard::ReadChar()
{
    if (charBuffer.size() > 0)
    {
        char c = charBuffer.front();
        charBuffer.pop();
        return c;
    }
    else
    {
        return 0;
    }
}

bool Keyboard::CharIsEmpty()
{
    return charBuffer.empty();
}

void Keyboard::FlushChar()
{
    charBuffer = std::queue<char>{};
}

void Keyboard::FlushAll()
{
    FlushKey();
    FlushChar();
}

void Keyboard::EnableAutoRepeat()
{
    AutorepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat()
{
    AutorepeatEnabled = false;
}

bool Keyboard::AutoRepeatIsEnabled()
{
    return AutorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode)
{
    keyStates[keycode] = true;
    keyBuffer.push(Keyboard::EventK(Keyboard::EventK::Type::Press, keycode));
    TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode)
{
    keyStates[keycode] = false;
    keyBuffer.push(Keyboard::EventK(Keyboard::EventK::Type::Release, keycode));
    TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(char character)
{
    charBuffer.push(character);
    TrimBuffer(charBuffer);
}

void Keyboard::ClearState()
{
    keyStates.reset();
}
