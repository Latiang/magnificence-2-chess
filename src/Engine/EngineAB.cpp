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
    Move move;
    //move.setUpgrade(5);
    principal_variation.push_back(move);
}