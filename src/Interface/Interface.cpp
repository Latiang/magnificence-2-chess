#include "Interface.h"

Interface::Interface(/* args */)
{
    setup();
}

Interface::~Interface()
{
}

void Interface::setup()
{
    //Register commands
    registerCommand(std::vector<std::string> {"exit", "quit"}, &CommandEngine::cmdQuit);
    registerCommand(std::vector<std::string> {"uci"}, &CommandEngine::cmdUCI);

    std::cout << WELCOME_MESSAGE << std::endl;

    //Do engine setup etc, magic tables...

    std::cout << "mgnf2: ";
}

void Interface::run()
{
    std::cout.setf(std::ios::unitbuf);
    std::string input;

    //Main loop
    while (!commandEngine.exit && std::getline(std::cin, input))
    {
        StringArguments arguments = StringArguments(input);
        executeCommand(arguments);

        if (commandEngine.interfaceMode == CommandEngine::DEBUG && !commandEngine.exit)
            std::cout << "mgnf2: ";
    }
}

void Interface::executeCommand(StringArguments arguments)
{
    auto it = commandMap.find(arguments.command);
    if (it != commandMap.end()) //Command has a mapped function
        //Execute the mapped function for this command
        (&commandEngine->*(it->second))(arguments);
    else if (commandEngine.interfaceMode == CommandEngine::DEBUG)
        std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
    

}

void Interface::registerCommand(const std::vector<std::string>& aliases, CommandEngine::CommandFunction function)
{
    for (auto alias: aliases) //Add a function pointer to the map for every alias
        commandMap.emplace(alias, function);
}