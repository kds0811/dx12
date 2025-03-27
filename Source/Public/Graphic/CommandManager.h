#pragma once
#include "GraphicCommonHeaders.h"

class CommandManager
{



public:
    CommandManager(ID3D12Device* device);
    ~CommandManager();

private:
    void Initialize(ID3D12Device* device);
};