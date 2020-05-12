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
#include <algorithm>

#include "../Board/BitBoard.h"

/**
 * @brief Base Chess Engine class, defines common variables and functions needed for children
 * 
 */
class Engine
{
public:
    //Inputs 
    int current_ply = 1;
    int max_depth = 0; //Max search depth specified by GUI/Interface
    int white_time = 1000000; //In milliseconds, time left in game
    int black_time = 1000000; //In milliseconds, time left in game
    int white_time_increment = 0; //In milliseconds, time incremented every move
    int black_time_increment = 0; //In milliseconds, time incremented every move
    int move_time = -1; //Search for exactly x milliseconds, UCI argument
    int max_nodes = -1; //Search max x nodes, UCI argument
    bool color = true;
    BitBoard board;
    bool stop_searching = false;


    //Output
    std::vector<Move> principal_variation; 

    //Main search function, is overriden by children
    virtual void search()
    {
        std::cout << "Base class, this should never trigger!" << std::endl;
    };

    Engine() {};
    ~Engine() {};
};