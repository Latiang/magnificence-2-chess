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

class Interface
{
private:
    /* data */
    std::unordered_map<std::string, CommandEngine::CommandFunction> commandMap;

    void registerCommand(const std::vector<std::string>& aliases, CommandEngine::CommandFunction function);
    void executeCommand(StringArguments arguments);
public:
    CommandEngine commandEngine = CommandEngine();

    Interface(/* args */);
    ~Interface();

    void setup();

    void run();
};