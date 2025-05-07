#include "PsoManager.h"
#include "Pso.h"
#include "ShaderManager.h"
#include "RootSignatureManager.h"

PsoManager::PsoManager() 
{
    mRootSignatureManager = std::make_unique<RootSignatureManager>();
    mShaderManager = std::make_unique<ShaderManager>();
}

PsoManager::~PsoManager() = default;

void PsoManager::BuildPso() 
{

}

