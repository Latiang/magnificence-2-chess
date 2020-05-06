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
    int currentPly = 1;
    int maxDepth = 0; //Max search depth specified by GUI/Interface
    int whiteTime = 1000000; //In milliseconds, time left in game
    int blackTime = 1000000; //In milliseconds, time left in game
    int whiteTimeIncrement = 0; //In milliseconds, time incremented every move
    int blackTimeIncrement = 0; //In milliseconds, time incremented every move
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