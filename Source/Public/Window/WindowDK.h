#pragma once
#include "WinDK.h"
#include <optional>
#include "Keyboard.h"
#include "Mouse.h"



class Window
{

private:
    class WindowClass
    {
    public:
        static const CHAR* GetName();
        static HINSTANCE GetInstance();
    private:
        WindowClass();
        ~WindowClass();
        WindowClass(const WindowClass&)            = delete;
        WindowClass& operator=(const WindowClass&) = delete;

        static const std::string wndClassName;
        static WindowClass wndClass;
        HINSTANCE hInstance;
    };

public:
    Window(int width, int height);
    ~Window();
    Window(const Window&)            = delete;
    Window& operator=(const Window&) = delete;
    const CHAR* GetTitle();
    void SetTitle(const std::string str);
    static std::optional<int> PrecessMessages();
    HWND GetHwnd();

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
    Keyboard kbd;
    Mouse mouse;

private:
    int width             = 0;
    int height            = 0;
    std::string titleName = "HZ PROJECT";
    HWND hWnd;
};
