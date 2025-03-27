#pragma once

class SceneManager
{
    static inline size_t mSceneObjectsCount = 0;


public:
    SceneManager();

    static inline size_t GetSceneObjectsCount() noexcept { return mSceneObjectsCount; }
};