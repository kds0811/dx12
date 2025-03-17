#pragma once
#include <string>


class Mesh
{
protected:
    std::string mName;
    int mID;

public:
    Mesh(std::string name, int ID)
        :
        mName(std::move(name)),
        mID(ID)
    {}

    [[nodiscard]] inline std::string GetName() { return mName; }
    [[nodiscard]] inline int GetID() noexcept { return mID; }

    inline void SetID(int id) noexcept { mID = id; }
    inline void SetName(std::string name) { mName = std::move(name); }
    
};