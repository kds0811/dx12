#include "MainController.h"
#include "WindowDK.h"

MainInputController::MainInputController(Window* wnd)
{
    mMouseController = std::make_unique<MouseController>(wnd);
    mKeyboardController = std::make_unique<KeyboardController>(wnd);
}

void MainInputController::Update()
{
    mMouseController->UpdateInput();
    mKeyboardController->UpdateInput();
}

bool MainInputController::KeyboardKeyIsPressed(unsigned char keyCode)
{
    return mKeyboardController->KeyIsPressed(keyCode);
}

bool MainInputController::MouseButtonIsPressed(unsigned char keyCode)
{
    return mMouseController->ButtonIsPressed(keyCode);
}

DirectX::XMFLOAT2 MainInputController::GetMousePositionOffset()
{
    return mMouseController->GetMousePosOffset();
}
