#include "App.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        App app;
        if (const auto ecode = app.Go())
        {
            return *ecode;
        }
    }
    catch (std::exception e)
    {
        MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR | MB_SETFOREGROUND);
    }
    catch (...)
    {
        MessageBoxA(nullptr, "unknown error", "ERROR", MB_ICONERROR | MB_SETFOREGROUND);
    }

    return -1;
}
