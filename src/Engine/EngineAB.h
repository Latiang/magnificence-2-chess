/**
 * @file EngineAB.h
 * @author William Sandström and Harald Bjurulf
 * @brief Contains the class EngineAlphaBeta
 * @version 0.1
 * @date 2020-05-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#include <iostream>

#include "Engine.h"
#include "../Interface/BoardConversions.h"

/**
 * @brief Alpha Beta Chess Engine class. Inherits from base class Engine.
 * 
 */
class EngineAlphaBeta : public Engine
{
public:
    void search() override;
    int eval(Move* moves_begin);
    int negamax(int depth, Move* moves_begin);
    int negamaxAB(int alpha, int beta, int depth, Move* moves_begin);
    int quiescence(int alpha, int beta, Move* moves_begin);
    EngineAlphaBeta(/* args */);
    ~EngineAlphaBeta();
};