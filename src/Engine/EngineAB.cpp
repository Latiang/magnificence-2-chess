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
    principal_variation.clear();
    principal_variation.push_back(Move());
}