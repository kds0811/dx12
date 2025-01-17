#pragma once

class Window;
class Camera;
class GameTimerW;

class CameraController
{
    Window* pWnd = nullptr;
    Camera* pCam = nullptr;
    GameTimerW* pTimer = nullptr;

public:
    CameraController(Window* wnd, Camera* cam, GameTimerW* timer);
    void UpdateKeyboardInput();

private:
    

};