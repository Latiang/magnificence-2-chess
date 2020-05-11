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

/**
 * @brief Alpha Beta Chess Engine class. Inherits from base class Engine.
 * 
 */
class EngineAlphaBeta : public Engine
{
public:
    Move moves[60];
    void search() override;
    int eval();
    int negamax(int depth);
    int negamaxAB(int alpha, int beta, int depth);
    EngineAlphaBeta(/* args */);
    ~EngineAlphaBeta();
};