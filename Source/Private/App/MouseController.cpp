#include "MouseController.h"
#include "WindowDK.h"

MouseController::MouseController(Window* wnd) : pWindow(wnd)
{
    unsigned char MouseButtons[] = {
        VK_LBUTTON,
        VK_RBUTTON,
        VK_MBUTTON
    };

    for (const auto& mb : MouseButtons)
    {
        mCurrentInput[mb] = false;
    }
}

void MouseController::UpdateInput() 
{
    // Update Mouse pressed buttons
    mCurrentInput[VK_LBUTTON] = pWindow->mouse.LIsPressed();
    mCurrentInput[VK_RBUTTON] = pWindow->mouse.RIsPressed();
    mCurrentInput[VK_MBUTTON] = pWindow->mouse.MIsPressed();

    // Update mouse Position
    DirectX::XMFLOAT2 CurrentMousePos = pWindow->mouse.GetPosVec();
    mMouseOffset = DirectX::XMFLOAT2(CurrentMousePos.x - mPrevMousePosition.x, CurrentMousePos.y - mPrevMousePosition.y);
    mPrevMousePosition = CurrentMousePos;
}

bool MouseController::ButtonIsPressed(unsigned char KeyCode) 
{
    if (mCurrentInput.contains(KeyCode))
    {
        return mCurrentInput[KeyCode];
    }
    return false;
}
