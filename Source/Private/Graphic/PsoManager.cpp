#include "PsoManager.h"
#include "Pso.h"

PsoManager::PsoManager() {}

PsoManager::~PsoManager() = default;

void PsoManager::Initialize() 
{
    mStandartPso = std::make_unique<Pso>();
}
