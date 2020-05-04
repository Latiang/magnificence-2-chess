#pragma once

#include "CommandEngine.h"

class Interface
{
private:
    /* data */
public:
    enum InterfaceMode {debug, uci};

    InterfaceMode mode = debug;
    CommandEngine commandEngine = CommandEngine();

    Interface(/* args */);
    ~Interface();

    void setup();

    void run();
};