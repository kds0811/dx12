#include "Window/WindowDK.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    Window wnd(800, 600);
    while (true)
    {
        if (const auto ecode = Window::PrecessMessages())
        {
            return *ecode;
        }
    }

    return 0;
}
