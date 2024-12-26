#include "Window/WindowDK.h"

const std::string Window::WindowClass::wndClassName = "HZ PROJECT";
Window::WindowClass Window::WindowClass::wndClass;

const CHAR* Window::WindowClass::GetName()
{
    return wndClassName.c_str();
}

HINSTANCE Window::WindowClass::GetInstance()
{
    return wndClass.hInstance;
}

Window::WindowClass::WindowClass() : hInstance(GetModuleHandle(nullptr))
{
    WNDCLASSEX wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = HandleMsgSetup;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetInstance();
    wcex.hIcon = nullptr;
    wcex.hCursor = nullptr;
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = GetName();
    wcex.hIconSm = nullptr;

    RegisterClassEx(&wcex);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(GetName(), hInstance);
}

Window::Window(int width, int height) : width(width), height(height)
{
    RECT rectWin;
    rectWin.left = 100;
    rectWin.right = rectWin.left + width;
    rectWin.top = 100;
    rectWin.bottom = rectWin.top + height;

    if (AdjustWindowRect(&rectWin, WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU, false) == 0)
    {
        // THROW_COM_ERROR_LAST("Adjust Window Rect ERROR");
    }

    hWnd = CreateWindowExA(0, WindowClass::GetName(), GetTitle(), WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
        rectWin.right - rectWin.left, rectWin.bottom - rectWin.top, nullptr, nullptr, WindowClass::GetInstance(), this);
    if (hWnd == nullptr)
    {
        // THROW_COM_ERROR_LAST("Creare Window ERROR");
    }

    UpdateWindow(hWnd);
    ShowWindow(hWnd, SW_SHOW);
}

Window::~Window()
{
    DestroyWindow(hWnd);
}

const CHAR* Window::GetTitle()
{
    return titleName.c_str();
}

void Window::SetTitle(const std::string str)
{
    if (SetWindowText(hWnd, str.c_str()) == 0)
    {
        // THROW_COM_ERROR_LAST("SET TITLE ERROR");
    }
}

std::optional<int> Window::PrecessMessages()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return static_cast<int>(msg.wParam);
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return {};
}

HWND Window::GetHwnd()
{
    return hWnd;
}

LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (msg)
    {
        case WM_CLOSE: PostQuitMessage(0); return 0;

        case WM_KILLFOCUS: kbd.ClearState(); break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnabled())
            {
                kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
            }
            break;

        case WM_KEYUP:
        case WM_SYSKEYUP: kbd.OnKeyReleased(static_cast<unsigned char>(wParam)); break;

        case WM_CHAR: kbd.OnChar(static_cast<unsigned char>(wParam)); break;

        case WM_MOUSEMOVE:
            POINTS pt = MAKEPOINTS(lParam);
            if (pt.x >= 0 && pt.x <= width && pt.y >= 0 && pt.y <= height)
            {
                mouse.OnMouseMove(pt.x, pt.y);

                if (!mouse.IsInWindow())
                {
                    SetCapture(hWnd);
                    mouse.OnMouseEnter();
                }
            }
            else
            {
                if (mouse.LIsPressed() || mouse.RIsPressed())
                {
                    mouse.OnMouseMove(pt.x, pt.y);
                }
                else
                {
                    ReleaseCapture();
                    mouse.OnMouseLeave();
                }
            }
            break;

        case WM_LBUTTONDOWN: mouse.OnLeftIsPressed(); break;

        case WM_LBUTTONUP: mouse.OnLeftIsReleased(); break;

        case WM_RBUTTONDOWN: mouse.OnRightIsPressed(); break;

        case WM_RBUTTONUP: mouse.OnRightIsReleased(); break;

        case WM_MOUSEWHEEL: int delta = GET_WHEEL_DELTA_WPARAM(wParam); mouse.OnWheelDeta(delta);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}