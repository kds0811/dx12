#include "App.h"
#include "GameTimer.h"



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    GameTimer timer;
    timer.Reset();
    auto TT = timer.GetTotalTime();
    auto DT = timer.GetDeltaTime();
    try
    {
        App app;
        while (true)
        {
            timer.Tick();
            if (const auto ecode = app.Go())
            {
                return *ecode;
            }
        }
    }
    catch (std::exception e)
    {
        MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR | MB_SETFOREGROUND);
    }
    catch (...)
    {
    }

    return -1;
}
