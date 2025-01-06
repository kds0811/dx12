#include "Window/WindowDK.h"
#include "App/App.h"

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
    WNDCLASSEX wcex = {};

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = HandleMsgSetup;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetInstance();
    wcex.hIcon = ::LoadIcon(hInstance, NULL);
    wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = GetName();
    wcex.hIconSm = ::LoadIcon(hInstance, NULL);

    RegisterClassEx(&wcex);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(GetName(), hInstance);
}

Window::Window(int width, int height, App* ownerApp) : Width(width), Height(height), pApp(ownerApp)
{
    int ScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    RECT WindowRect = {0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height)};
    ::AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

    int WindowWidth = WindowRect.right - WindowRect.left;
    int WindowHeight = WindowRect.bottom - WindowRect.top;

    int WindowX = std::max<int>(0, (ScreenWidth - WindowWidth) / 2);
    int WindowY = std::max<int>(0, (ScreenHeight - WindowHeight) / 2);

    hWnd = CreateWindowExA(0, WindowClass::GetName(), GetTitle().c_str(), WS_OVERLAPPEDWINDOW, WindowX, WindowY, WindowWidth, WindowHeight,
        nullptr, nullptr, WindowClass::GetInstance(), this);

    assert(hWnd && "Failed to create window");

    UpdateWindow(hWnd);
    ShowWindow(hWnd, SW_SHOW);
}

Window::~Window()
{
    DestroyWindow(hWnd);
}

std::string Window::GetTitle()
{
    return titleName;
}

void Window::SetTitle(const std::string str)
{
    if (SetWindowText(hWnd, str.c_str()) == 0)
    {
        assert(hWnd && "SET TITLE ERROR");
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

    return std::nullopt;
}

HWND Window::GetHwnd() const
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
        {
            if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnabled())
            {
                kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
            }
            break;
        }

        case WM_KEYUP:
        case WM_SYSKEYUP: kbd.OnKeyReleased(static_cast<unsigned char>(wParam)); break;

        case WM_CHAR: kbd.OnChar(static_cast<unsigned char>(wParam)); break;

        case WM_MOUSEMOVE:
        {
            POINTS pt = MAKEPOINTS(lParam);
            if (pt.x >= 0 && pt.x <= Width && pt.y >= 0 && pt.y <= Height)
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
        }

        case WM_LBUTTONDOWN: mouse.OnLeftIsPressed(); break;

        case WM_LBUTTONUP: mouse.OnLeftIsReleased(); break;

        case WM_RBUTTONDOWN: mouse.OnRightIsPressed(); break;

        case WM_RBUTTONUP: mouse.OnRightIsReleased(); break;

        case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            mouse.OnWheelDeta(delta);
            break;
        }

        case WM_ACTIVATE:
        {
            if (LOWORD(wParam) == WA_INACTIVE)
            {
                pApp->OnStop();
            }
            else
            {
                pApp->OnStart();
            }
            break;
        }
        case WM_ENTERSIZEMOVE:
        {
            pApp->OnStop();
            break;
        }
        case WM_EXITSIZEMOVE:
        {
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            UINT nWidth = static_cast<UINT>(clientRect.right - clientRect.left);
            UINT nHeight = static_cast<UINT>(clientRect.bottom - clientRect.top);

            if (nWidth == Width && nHeight == Height)
            {
                pApp->OnStart();
            }
            else
            {
                Width = nWidth;
                Height = nHeight;
                pApp->OnResize(Width, Height);
                pApp->OnStart();
            }

            break;
        }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}