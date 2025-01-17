#include "CameraController.h"
#include "Camera.h"
#include "WindowDK.h"
#include "GameTimerW.h"

CameraController::CameraController(Window* wnd, Camera* cam, GameTimerW* timer) : pWnd(wnd), pCam(cam), pTimer(timer) {}


void CameraController::UpdateKeyboardInput()
{
    const float dt = pTimer->GetDeltaTime();

    if (pWnd->kbd.KeyIsPressed('W'))
    {
        pCam->MoveForward(1.0f, dt);
    }
    if (pWnd->kbd.KeyIsPressed('S'))
    {
        pCam->MoveForward(-1.0f, dt);
    }
    if (pWnd->kbd.KeyIsPressed('D'))
    {
        pCam->MoveRight(1.0f, dt);
    }
    if (pWnd->kbd.KeyIsPressed('A'))
    {
        pCam->MoveRight(-1.0f, dt);
    }
    if (pWnd->kbd.KeyIsPressed('E'))
    {
        pCam->MoveAbsoluteUp(1.f, dt);
    }
    if (pWnd->kbd.KeyIsPressed('Q'))
    {
        pCam->MoveAbsoluteUp(-1.f, dt);
    }
}