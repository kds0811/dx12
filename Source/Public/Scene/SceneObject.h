#pragma once
#include <cstdint>
#include <string>

class SceneObject
{
public:
    SceneObject(int id, std::string name) : mID(id), mName(name) {}
    virtual ~SceneObject() = default;

    virtual void Update(float dt) = 0;

    inline void SetName(std::string name) { mName = name; }
    inline std::string GetName() { return mName.empty() ? " Unnamed Object " : mName; }
    inline void SetId(int id) { mID = id; }
    inline int GetID() { return mID; }

protected:
    int mID;
    std::string mName;
};