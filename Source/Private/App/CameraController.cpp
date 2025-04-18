#include "CameraController.h"
#include "Camera.h"
#include "WindowDK.h"
#include "GameTimerW.h"
#include "MainController.h"
#include <DirectXMath.h>


CameraController::CameraController(Camera* cam, MainInputController* controller)
{
    pCam = cam;
    pController = controller;
}

void CameraController::UpdateInput()
{
    UpdateKeyboardInput();
    UpdateMouseInput();
}

void CameraController::UpdateKeyboardInput()
{
    if (pController->KeyboardKeyIsPressed('W'))
    {
        pCam->MoveForward(1.0f);
    }
    if (pController->KeyboardKeyIsPressed('S'))
    {
        pCam->MoveForward(-1.0f);
    }
    if (pController->KeyboardKeyIsPressed('D'))
    {
        pCam->MoveRight(1.0f);
    }
    if (pController->KeyboardKeyIsPressed('A'))
    {
        pCam->MoveRight(-1.0f);
    }
    if (pController->KeyboardKeyIsPressed('E'))
    {
        pCam->MoveAbsoluteUp(1.f);
    }
    if (pController->KeyboardKeyIsPressed('Q'))
    {
        pCam->MoveAbsoluteUp(-1.f);
    }
}

void CameraController::UpdateMouseInput()
{
   // CheckMouseRightButtonIsPressed();

    if (pController->MouseButtonIsPressed(VK_RBUTTON))
    {
        auto MouseOfsset = pController->GetMousePositionOffset();
        pCam->RotateCamera(MouseOfsset.x, MouseOfsset.y);
    }
}
//
//void CameraController::CheckMouseRightButtonIsPressed()
//{
//
//    if (pWnd->mouse.RIsPressed() && !bMouseRightButtonIsPressed)
//    {
//        bMouseRightButtonIsPressed = true;
//        PrevMousePosition = pWnd->mouse.GetPosVec();
//    }
//    
//    if (bMouseRightButtonIsPressed && !pWnd->mouse.RIsPressed())
//    {
//        bMouseRightButtonIsPressed = false;
//        PrevMousePosition = {0.0f, 0.0f};
//    }
//}
