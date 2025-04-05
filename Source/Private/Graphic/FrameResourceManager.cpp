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

FrameResourceManager::FrameResourceManager(FrameResourceManager&& other) noexcept
    :
    mFrameResources(std::move(other.mFrameResources)),
    mCurrFrameResource(std::move(other.mCurrFrameResource)),
    mCurrFrameResourceIndex(other.mCurrFrameResourceIndex),
    mNumFrameResources(other.mNumFrameResources)
{}

FrameResourceManager& FrameResourceManager::operator=(FrameResourceManager&& other) noexcept
{
    mFrameResources = std::move(other.mFrameResources);
    mCurrFrameResource = std::move(other.mCurrFrameResource);
    mCurrFrameResourceIndex = other.mCurrFrameResourceIndex;
    mNumFrameResources = other.mNumFrameResources;
    return *this;
}

void FrameResourceManager::CycleToNextFrameResource()
{
    mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % mNumFrameResources;
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
}

void FrameResourceManager::BuildFrameResources()
{
    mNumFrameResources = Settings::mNumFrameResources;
    for (size_t i = 0; i < mNumFrameResources; ++i)
    {
        mFrameResources.push_back(
            std::make_unique<FrameResource>(Device::GetDevice(), mPassConstantCount, SceneManager::GetSceneObjectsCount(), MaterialManager::GetMaterialCount()));
    }

    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();
    LOG_MESSAGE("Frame resources is builded");
}

bool FrameResourceManager::CurrFrameResourceIsValid()
{
    assert(mCurrFrameResource);
    if (!mCurrFrameResource)
    {
        LOG_ERROR("Current Frame Resource pointer is nullptr");
        return false;
    }
    else
    {
        return true;
    }
}
