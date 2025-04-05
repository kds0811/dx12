#pragma once
#include "FrameResource.h"

class FrameResourceManager
{
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    UINT mCurrFrameResourceIndex = 0;
    static constexpr UINT mPassConstantCount = 2;

    // Cache number of frame resources
    UINT mNumFrameResources = 0;

public:
    FrameResourceManager();
    ~FrameResourceManager();
    FrameResourceManager(const FrameResourceManager&) = delete;
    FrameResourceManager& operator=(const FrameResourceManager&) = delete;
    FrameResourceManager(FrameResourceManager&& other) noexcept;
    FrameResourceManager& operator=(FrameResourceManager&& other) noexcept;

    void CycleToNextFrameResource();

    [[nodiscard]] inline UploadBuffer<PassConstants>* GetCurrentPassConstants()
    {
        if (!CurrFrameResourceIsValid())
        {
            return nullptr;
        }
        return mCurrFrameResource->GetPassConstants();
    }

    [[nodiscard]] inline UploadBuffer<MaterialConstants>* GetCurrentMaterialConstants()
    {
        if (!CurrFrameResourceIsValid())
        {
            return nullptr;
        }
        return mCurrFrameResource->GetMaterialConstants();
    }

    [[nodiscard]] inline UploadBuffer<ObjectConstants>* GetCurrentObjectConstants()
    {
        if (!CurrFrameResourceIsValid())
        {
            return nullptr;
        }
        return mCurrFrameResource->GetObjectConstants();
    }


private:
    void BuildFrameResources();
    bool CurrFrameResourceIsValid();
};
