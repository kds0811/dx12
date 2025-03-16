#pragma once
#include <string>


class Mesh
{
protected:
    std::string mName;
    int mID;

public:
    Mesh() = default;

    void SetName(std::string name) { mName = std::move(name); }
    std::string GetName() { return mName; }

    void SetID(int id) { mID = id; }
    int GetID() { return mID; }
};