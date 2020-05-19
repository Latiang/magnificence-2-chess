/**
 * @file EngineAB.h
 * @author William Sandström and Harald Bjurulf
 * @brief Contains the class EngineMCT
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
 * @brief Monte Carlo Engine class. Inherits from base class Engine.
 * 
 */
class EngineMCT : public Engine
{
public:

    void search() override;

    EngineMCT(/* args */);
    ~EngineMCT();
};