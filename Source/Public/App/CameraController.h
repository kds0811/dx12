#pragma once
#include <DirectXMath.h>


class Window;
class Camera;
class MainInputController;

class CameraController
{
    MainInputController* pController = nullptr;
    Camera* pCam = nullptr;



public:
    CameraController(Camera* cam, MainInputController* controller);
    void UpdateInput();

private:
    void UpdateKeyboardInput();
    void UpdateMouseInput();

   // void CheckMouseRightButtonIsPressed();

};