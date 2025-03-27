#include "FrameResourceManager.h"
#include "Settings.h"
#include "MaterialManager.h"
#include "Device.h"
#include "SceneManager.h"

FrameResourceManager::FrameResourceManager()
{
    BuildFrameResources();
}

void FrameResourceManager::BuildFrameResources() 
{
    for (int i = 0; i < Settings::mNumFrameResources; ++i)
    {
        mFrameResources.push_back(
            std::make_unique<FrameResource>(Device::GetDevice(), mPassConstantCout, (UINT)SceneManager::GetSceneObjectsCount(), (UINT)MaterialManager::GetMaterialCount()));
    }
}
