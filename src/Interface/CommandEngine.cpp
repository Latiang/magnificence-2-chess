#include "CommandEngine.h"

CommandEngine::CommandEngine(/* args */)
{
    //std::atomic<int> test{1};
}

CommandEngine::~CommandEngine()
{
}

//Debug commands
void CommandEngine::cmdDisplay(StringArguments& arguments)
{

}

void CommandEngine::cmdQuit(StringArguments& arguments)
{
    //cmd: quit. Exit the program
    std::cout << "Exiting Magnficence 2..." << std::endl;
    exit = true;
}

void CommandEngine::cmdPerft(StringArguments& arguments)
{
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;
    

}

void CommandEngine::cmdFen(StringArguments& arguments)
{

}

//UCI commands
void CommandEngine::cmdGo(StringArguments& arguments)
{
    if (currentlySearching)
    {
        errorMessage("The engine is already searching");
        return;
    }
    currentlySearching = true;
    std::thread searchThread(&CommandEngine::runSearch, this, std::ref(mainEngine));
    searchThread.detach();

}

void CommandEngine::cmdUCI(StringArguments& arguments)
{
    //cmd: uci. Enter UCI mode
    interfaceMode = UCI;
    std::cout << "id name " << ENGINE_NAME << std::endl;
	std::cout << "id author " << AUTHOR_NAME << std::endl;
	std::cout << "uciok" << std::endl;

}

void CommandEngine::cmdPosition(StringArguments& arguments)
{

}

void CommandEngine::cmdSetBoard(StringArguments& arguments)
{

}

void CommandEngine::cmdStop(StringArguments& arguments)
{

}

void CommandEngine::cmdIsReady(StringArguments& arguments)
{
    //cmd: isready. Replies with readyok if engine is ready for new commands
    std::cout << "readyok" << std::endl;   
}

//Helper functions
bool CommandEngine::areArgumentsCorreclyFormatted(StringArguments& arguments, int size)
{
    if (arguments.arguments.size() < size)
    {
        errorMessage("Incorrect argument count");
        return false;
    }
    else
        return true;
}

void CommandEngine::errorMessage(std::string message)
{
    if (interfaceMode == TESTING)
        std::cout << "The command failed due to: " << message << std::endl;
}

void CommandEngine::runSearch(Engine& engine)
{ 
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    
    engine.search();

    currentlySearching = false;

    if (interfaceMode == TESTING)
        std::cout << "mgnf2: ";
}