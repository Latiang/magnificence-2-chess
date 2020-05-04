#pragma once
#include <iostream>

#include "Engine.h"

class EngineAB : public Engine
{
private:
    /* data */
public:
    int test = 15;
    void search() override;
    EngineAB(/* args */);
    ~EngineAB();
};