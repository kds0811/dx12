#pragma once
#include "WindowInfo.h"
#include <optional>
#include "Keyboard.h"
#include "Mouse.h"


class App;

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
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;

        static const std::string wndClassName;
        static WindowClass wndClass;
        HINSTANCE hInstance;
    };

public:
    Window(int width, int height, App* ownerApp);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    std::string GetTitle();
    void SetTitle(const std::string str) const;
    static std::optional<int> PrecessMessages();
    HWND GetHwnd() const;

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
    Keyboard kbd;
    Mouse mouse;

private:
    UINT Width = 0;
    UINT Height = 0;
    std::string titleName = "DX 12";
    HWND hWnd;
    App* pApp = nullptr;
    
};
