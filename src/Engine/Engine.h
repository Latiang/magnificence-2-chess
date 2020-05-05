/**
 * @file Engine.h
 * @author William Sandström and Harald Bjurulf
 * @brief Contains the Engine base class
 * @version 0.1
 * @date 2020-05-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <vector>
#include <iostream>

#include "../Board/BitBoard.h"

/**
 * @brief Base Chess Engine class, defines common variables and functions needed for children
 * 
 */
class Engine
{
public:
    //Inputs 
    int maxDepth = 0;
    bool color = true;
    BitBoard board;
    bool stopSearching = false;

    //Output
    std::vector<Move> principalVariation; 

    //Main search function, is overriden by children
    virtual void search()
    {
        std::cout << "Base class, this should never trigger!" << std::endl;
    };

    Engine() {};
    ~Engine() {};
};