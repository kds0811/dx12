#pragma once
#include <DirectXMath.h>


class Window;
class Camera;
class GameTimerW;

class CameraController
{
    Window* pWnd = nullptr;
    Camera* pCam = nullptr;
    GameTimerW* pTimer = nullptr;
    DirectX::XMFLOAT2 PrevMousePosition;
    bool bMouseRightButtonIsPressed = false;

public:
    CameraController(Window* wnd, Camera* cam, GameTimerW* timer);
    void UpdateInput();

private:
    void UpdateKeyboardInput();
    void UpdateMouseInput();

    void CheckMouseRightButtonIsPressed();

};