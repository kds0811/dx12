#include "PsoManager.h"
#include "Pso.h"

PsoManager::PsoManager() 
{
    mRootSignatureManager = std::make_unique<RootSignatureManager>();
}

PsoManager::~PsoManager() = default;

void PsoManager::Initialize() 
{
    mStandartPso = std::make_unique<Pso>();
}
