#include "Interface.h"

// @brief Initialize the class by calling setup()
Interface::Interface(/* args */)
{
    setup();
}

Interface::~Interface()
{
}

// @brief Register program commands by connecting the command string (and aliases) to their respective function pointers
void Interface::setup()
{
    //Register commands
    //Engine related commands
    registerCommand(std::vector<std::string> {"exit", "quit"}, &CommandEngine::cmdQuit);
    registerCommand(std::vector<std::string> {"help", "h"}, &CommandEngine::cmdHelp);
    registerCommand(std::vector<std::string> {"perft", "peft", "preft", "pertf"}, &CommandEngine::cmdPerft);
    registerCommand(std::vector<std::string> {"go", "run", "play", "search"}, &CommandEngine::cmdGo);
    registerCommand(std::vector<std::string> {"display", "disp", "d"}, &CommandEngine::cmdDisplay);
    registerCommand(std::vector<std::string> {"move", "mv"}, &CommandEngine::cmdMove);
    registerCommand(std::vector<std::string> {"position", "pos", "setboard", "sb"}, &CommandEngine::cmdPosition);
    registerCommand(std::vector<std::string> {"fen"}, &CommandEngine::cmdFen);
    registerCommand(std::vector<std::string> {"selfplay"}, &CommandEngine::cmdSelfPlay);
    registerCommand(std::vector<std::string> {"moves", "mvs", "legalmoves"}, &CommandEngine::cmdLegalMoves);
    registerCommand(std::vector<std::string> {"divide", "dv", "div"}, &CommandEngine::cmdDivide);
    registerCommand(std::vector<std::string> {"unmove", "umv"}, &CommandEngine::cmdUnmove);

    //Machine learning related commands
    registerCommand(std::vector<std::string> {"train", "tr"}, &CommandEngine::cmdTrain);
    registerCommand(std::vector<std::string> {"loadmodel", "lm"}, &CommandEngine::cmdLoadModel);
    registerCommand(std::vector<std::string> {"modelmove", "mm"}, &CommandEngine::cmdModelMove);
    registerCommand(std::vector<std::string> {"resetmodelcheckpoints", "resetcheckpoints", "rc", "rmc"}, &CommandEngine::cmdResetModelCheckpoints);
    registerCommand(std::vector<std::string> {"modelmoves", "mms"}, &CommandEngine::cmdModelMoves);
    registerCommand(std::vector<std::string> {"modeltrainingperft", "trainingperft", "modelperft", "tperft"}, &CommandEngine::cmdModelLearnPerft);
    registerCommand(std::vector<std::string> {"modeloutput", "mo"}, &CommandEngine::cmdModelDisplayOutput);
    registerCommand(std::vector<std::string> {"modeleval", "me"}, &CommandEngine::cmdModelBoardValue);

    //UCI related commands
    registerCommand(std::vector<std::string> {"uci"}, &CommandEngine::cmdUCI);
    registerCommand(std::vector<std::string> {"isready"}, &CommandEngine::cmdIsReady);
    registerCommand(std::vector<std::string> {"stop"}, &CommandEngine::cmdStop);

    std::cout << WELCOME_MESSAGE << std::endl;

    //Do engine setup etc, magic tables...

    std::cout << "mgnf2: ";
}

// @brief Main command loop. Takes input and executes the respective command functions by using the command map
void Interface::run()
{
    std::cout.setf(std::ios::unitbuf); //UCI standard compliance
    std::string input;

    //Main loop
    while (!command_engine.exit && std::getline(std::cin, input))
    {
        StringArguments arguments = StringArguments(input);
        executeCommand(arguments);

        if (command_engine.interface_mode == CommandEngine::TESTING && !command_engine.exit && !command_engine.currently_searching)
            std::cout << "mgnf2: "; //Makes command interface look nicer, not allowed in UCI mode
    }
}

// @brief Execute a command function from the command map if it exists
void Interface::executeCommand(StringArguments arguments)
{
    auto it = command_map.find(arguments.command);
    if (it != command_map.end()) //Command has a mapped function
        //Execute the mapped function for this command
        (&command_engine->*(it->second))(arguments);
    else if (command_engine.interface_mode == CommandEngine::TESTING)
        std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl; //Only allowed in non UCI mode
    

}

// @brief Register a command by adding the command string and respective command fuction to the function map
void Interface::registerCommand(const std::vector<std::string>& aliases, CommandEngine::CommandFunction function)
{
    for (auto alias: aliases) //Add a function pointer to the map for every alias
        command_map.emplace(alias, function);
}