#pragma once
#include <iostream>

#include "StringHelpers.h"

const std::string ENGINE_NAME = "Magnificence2";
const std::string AUTHOR_NAME = "ProgBoys";

class CommandEngine
{
private:
    /* data */
public:
    typedef void(CommandEngine::*CommandFunction)(StringArguments&);

    bool exit = false;

    CommandEngine(/* args */);
    ~CommandEngine();

    //Debug Commands
    void cmdDisplay(StringArguments& arguments);
    void cmdQuit(StringArguments& arguments);
    void cmdPerft(StringArguments& arguments);
    void cmdFen(StringArguments& arguments);

    //UCI commands
    void cmdGo(StringArguments& arguments);
    void cmdUCI(StringArguments& arguments);
    void cmdPosition(StringArguments& arguments);
    void cmdSetBoard(StringArguments& arguments);
    void cmdStop(StringArguments& arguments);
    void cmdIsReady(StringArguments& arguments);
};
