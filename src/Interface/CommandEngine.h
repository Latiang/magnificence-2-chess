#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#include "StringHelpers.h"
#include "BoardConversions.h"
#include "../Engine/EngineAB.h"
#include "../Engine/Engine.h"
#include "../Board/BitBoard.h"

const std::string ENGINE_NAME = "Magnificence2";
const std::string AUTHOR_NAME = "ProgBoys";

const std::string HELP_STRING = 
    "Command list: \n"
    "help                   Display a list of commands\n"
    "quit                   Exit the program\n"
    "display                Display the board in the console \n"
    "go         [depth]     Search with the current Engine\n"
    "perft      <depth>     Calculate perft for current position\n"
    "divide     <depth>     Perft score per each legal move of current position.\n"
    "fen                    Display the fen string for the current board\n"
    "setboard   <fen>       Set the board to a fen string\n"
    "move       <move>      Peform an algebraic move\n"
    "moves                  List legal moves\n"
    "selfplay               Play a game between two Engines\n"
    "uci                    Enter UCI mode\n";


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
    void cmdHelp(StringArguments& arguments);
    void cmdQuit(StringArguments& arguments);
    void cmdPerft(StringArguments& arguments);
    void cmdFen(StringArguments& arguments);
    void cmdSelfPlay(StringArguments& arguments);
    void cmdDivide(StringArguments& arguments);
    void cmdMove(StringArguments& arguments);
    void cmdLegalMoves(StringArguments& arguments);

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
