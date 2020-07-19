/**
 * @file CommandEngine.h
 * @author William Sandström and Harald Bjurulf
 * @brief This file contains the CommandEngine class
 * @version 0.1
 * @date 2020-05-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#include "StringHelpers.h"
#include "BoardConversions.h"
#include "../Engine/EngineAB.h"
#include "../Engine/EngineMCT.h"
#include "../Engine/Engine.h"
#include "../Board/BitBoard.h"
#include "../Model/PolicyModel.h"
#include "../Model/TrainingHelper.h"

//Constants which represent required fields for the UCI standard
const std::string ENGINE_NAME = "Magnificence2";
const std::string AUTHOR_NAME = "ProgBoys";

//Command list for the help command
const std::string HELP_STRING = 
    "Command list: \n"
    "Engine commands:\n"
    "help                   Display a list of commands\n"
    "quit                   Exit the program\n"
    "display                Display the board in the console \n"
    "go         [depth]     Search with the current Engine\n"
    "perft      <depth>      Calculate perft for current position\n"
    "divide     <depth>     Perft score per each legal move of current position.\n"
    "fen                    Display the fen string for the current board\n"
    "position   <fen/sp>    Set the board to a fen string/startpos\n"
    "move       <move>      Peform an algebraic move\n"
    "moves                  List legal moves\n"
    "unmove <move>          Undo an algebraic move\n"
    "selfplay               Play a game between two Engines\n"
    "uci                    Enter UCI mode\n"
    "Machine Learning commmands:\n"
    "train                  Train the model\n"
    "modelmove              Make the top rated move from the model output\n"
    "modelmoves [cutoff]    List all output moves above a certain float cutoff value\n"
    "modeloutput            List all the output neuron values for this position\n"
    "loadmodel [cp/'r']     Load the current model from file, or specify a checkpoint to load. 'r' loads the most recent checkpoint.\n"
    "resetcheckpoints       Remove all checkpoints for the current model. Warning: cannot be undone\n"
    "tperft <depth>         Train the model to predict move gen using perft of specified depth\n";

const std::string STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * @brief This class contains functions related to executing various commands sent from the Interface.
 * @details The command functions in this class are used by the Interface through function pointers. 
 * All the command functions recieve a StringArguments struct which contain information regarding
 * the input that the interface recieved to trigger this function, ie flags and arguments and such.
 * The CommandEngine has ownership of the main chess Engine as well as a side Engine (which is used for selfplay testing)
 * It also has control of a separate thread which is used to initiate searching in the main engine. This is needed as the UCI
 * standard requires responsiveness during searching.
 */
class CommandEngine
{
private:
    //Chess engines/players
    EngineMCT main_engine; //The main engine, player
    EngineAlphaBeta side_engine; //For the second player in self play

    PolicyModel model;

    void runSearch(Engine& engine); //Run search on an engine, is used by a second thread

public:

    enum InterfaceMode {TESTING, UCI};
    typedef void(CommandEngine::*CommandFunction)(StringArguments&); //Function pointer to commmand functions

    //Variables which the main loop in Interface check
    bool exit = false;
    InterfaceMode interface_mode = TESTING;
    std::atomic<bool> currently_searching{false}; //Boolean which is accessed from the second search thread.

    CommandEngine(/* args */);
    ~CommandEngine();

    //Debug commands
    void cmdDisplay(StringArguments& arguments);
    void cmdHelp(StringArguments& arguments);
    void cmdQuit(StringArguments& arguments);
    void cmdPerft(StringArguments& arguments);
    void cmdFen(StringArguments& arguments);
    void cmdSelfPlay(StringArguments& arguments);
    void cmdDivide(StringArguments& arguments);
    void cmdMove(StringArguments& arguments);
    void cmdLegalMoves(StringArguments& arguments);
    void cmdUnmove(StringArguments& arguments);

    //UCI commands
    void cmdGo(StringArguments& arguments);
    void cmdUCI(StringArguments& arguments);
    void cmdPosition(StringArguments& arguments);
    void cmdSetBoard(StringArguments& arguments);
    void cmdStop(StringArguments& arguments);
    void cmdIsReady(StringArguments& arguments);

    //Machine learning commands
    void cmdTrain(StringArguments& arguments);
    void cmdLoadModel(StringArguments& arguments);
    void cmdResetModelCheckpoints(StringArguments& arguments);

    void cmdModelLearnPerft(StringArguments& arguments);
    void cmdModelMove(StringArguments& arguments);
    void cmdModelMoves(StringArguments& arguments);
    void cmdModelDisplayOutput(StringArguments& arguments);
    void cmdModelBoardValue(StringArguments& arguments);

    bool areArgumentsCorreclyFormatted(StringArguments& arguments, int size);
    void errorMessage(std::string message);
    bool askForConfirmation();
};
