#include "EngineAB.h"

EngineAlphaBeta::EngineAlphaBeta()
{

}

EngineAlphaBeta::~EngineAlphaBeta()
{
}

/// @brief Alpha Beta prototype search
void EngineAlphaBeta::search()
{
    principalVariation.clear();
    principalVariation.push_back(Move());
}