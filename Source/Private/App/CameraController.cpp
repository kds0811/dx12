#include "CameraController.h"
#include "Camera.h"
#include "WindowDK.h"
#include "GameTimerW.h"
#include "PixProfile.h"


#if defined PIXPROFILE
#define USE_PIX
#include <pix3.h>
#endif

CameraController::CameraController(Window* wnd, Camera* cam, GameTimerW* timer)
{
    assert(wnd);
    assert(cam);
    assert(timer);
    pWnd = wnd;
    pCam = cam;
    pTimer = timer;
}

void CameraController::UpdateInput()
{

#if defined PIXPROFILE
    PIXScopedEvent(PIX_COLOR(60, 150, 60), L"Update Camera Input");
#endif

    UpdateKeyboardInput();
    UpdateMouseInput();
}

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

void CameraController::UpdateMouseInput()
{
    CheckMouseRightButtonIsPressed();

    if (bMouseRightButtonIsPressed)
    {
        DirectX::XMFLOAT2 CurrentMousePos = pWnd->mouse.GetPosVec();
        float Xofsset = CurrentMousePos.x - PrevMousePosition.x;
        float Yofsset = CurrentMousePos.y - PrevMousePosition.y;
        pCam->RotateCamera(Xofsset, Yofsset, pTimer->GetDeltaTime());
        PrevMousePosition = CurrentMousePos;
    }

}

void CameraController::CheckMouseRightButtonIsPressed()
{

    if (pWnd->mouse.RIsPressed() && !bMouseRightButtonIsPressed)
    {
        bMouseRightButtonIsPressed = true;
        PrevMousePosition = pWnd->mouse.GetPosVec();
    }
    
    if (bMouseRightButtonIsPressed && !pWnd->mouse.RIsPressed())
    {
        bMouseRightButtonIsPressed = false;
        PrevMousePosition = {0.0f, 0.0f};
    }
}
