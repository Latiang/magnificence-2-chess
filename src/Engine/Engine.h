#pragma once

#include <vector>
#include <iostream>

#include "../Board/BitBoard.h"

class Engine
{
private:
    /* data */
public:
    //Inputs 
    int maxDepth = 0;
    bool color = true;
    BitBoard board;
    bool stopSearching = false;

    //Output
    std::vector<u32> principalVariation; 

    virtual void search()
    {
        std::cout << "Base class, this should never trigger!" << std::endl;
    };

    Engine(/* args */) {};
    ~Engine() {};
};