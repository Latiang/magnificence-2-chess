/**
 * @file Interface.h
 * @author William Sandström and Harald Bjurulf
 * @brief This file contains the Interface class
 * @version 0.1
 * @date 2020-05-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <iostream>
#include <thread>
#include <sstream>
#include <string>
#include <unordered_map>

#include "CommandEngine.h"
#include "StringHelpers.h"

const std::string WELCOME_MESSAGE = 
    "--------------------------------\n"
	"Magnificence 2 Development Build\n"
	"--------------------------------\n";

/**
 * @brief The main Interface class for the program. 
 * It controls the main command loop, reads input and executes commands by calling functions in CommandEngine.
 */
class Interface
{
private:
    //A map of command strings to command functions (from CommandEngine)
    std::unordered_map<std::string, CommandEngine::CommandFunction> command_map;

    //Functions relating to setup and execution of commands in commandMap
    void registerCommand(const std::vector<std::string>& aliases, CommandEngine::CommandFunction function);
    void executeCommand(StringArguments arguments);
public:
    CommandEngine command_engine;

    Interface(/* args */);
    ~Interface();

    //Called on initialization
    void setup();

    //Main command loop. Reads input, executes command functions in CommandEngine
    void run();
};