#pragma once
#include "../Board/BitBoard.h"

struct SearchState
{

};

class Engine
{
private:
    /* data */
public:
    int maxDepth = 0;
    bool color = true;
    //BitBoard board;

    virtual void search();

    Engine(/* args */);
    ~Engine();
};