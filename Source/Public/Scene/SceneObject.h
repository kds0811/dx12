#pragma once
#include <cstdint>
#include <string>

class SceneObject
{
public:
    SceneObject() = default;
    virtual ~SceneObject() = default;

    virtual void Update(float dt) = 0;

    inline void SetName(std::string name) { mName = name; }
    inline std::string GetName() { return mName; }
    inline void SetId(int id) { mID = id; }
    inline std::uint64_t GetID() { return mID; }

protected:
    std::uint64_t mID;
    std::string mName;
};