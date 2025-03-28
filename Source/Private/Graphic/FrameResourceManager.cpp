#include "FrameResourceManager.h"
#include "Settings.h"
#include "MaterialManager.h"
#include "Device.h"
#include "SceneManager.h"


FrameResourceManager::FrameResourceManager()
{
    BuildFrameResources();
}

FrameResourceManager::~FrameResourceManager() = default;

void FrameResourceManager::CycleToNextFrameResource()
{
    mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % mNumFrameResources;
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
}

void FrameResourceManager::BuildFrameResources() 
{
    mNumFrameResources = Settings::mNumFrameResources;
    for (int i = 0; i < mNumFrameResources; ++i)
    {
        mFrameResources.push_back(
            std::make_unique<FrameResource>(Device::GetDevice(), mPassConstantCount, (UINT)SceneManager::GetSceneObjectsCount(), (UINT)MaterialManager::GetMaterialCount()));
    }

    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
    LOG_MESSAGE("Frame resources is builded");
}
