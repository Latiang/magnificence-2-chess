#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#include "StringHelpers.h"
#include "../Engine/EngineAB.h"
#include "../Engine/Engine.h"

const std::string ENGINE_NAME = "Magnificence2";
const std::string AUTHOR_NAME = "ProgBoys";

class CommandEngine
{
private:
    /* data */
    //Engine searching
    //EngineAB sideEngine; //For the second player in self play

    //std::vector<int> test;
    void runSearch(Engine& engine);

public:
    EngineAB mainEngine; //The main engine, player
    std::atomic<bool> currentlySearching{false};
    enum InterfaceMode {TESTING, UCI};
    typedef void(CommandEngine::*CommandFunction)(StringArguments&);

    bool exit = false;
    InterfaceMode interfaceMode = TESTING;

    CommandEngine(/* args */);
    ~CommandEngine();

    //Debug Commands
    void cmdDisplay(StringArguments& arguments);
    void cmdQuit(StringArguments& arguments);
    void cmdPerft(StringArguments& arguments);
    void cmdFen(StringArguments& arguments);
    void cmdSelfPlay(StringArguments& arguments);

    //UCI commands
    void cmdGo(StringArguments& arguments);
    void cmdUCI(StringArguments& arguments);
    void cmdPosition(StringArguments& arguments);
    void cmdSetBoard(StringArguments& arguments);
    void cmdStop(StringArguments& arguments);
    void cmdIsReady(StringArguments& arguments);

    bool areArgumentsCorreclyFormatted(StringArguments& arguments, int size);
    void errorMessage(std::string message);
};
