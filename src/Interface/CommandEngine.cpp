#include "CommandEngine.h"

CommandEngine::CommandEngine(/* args */)
{
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

}
void CommandEngine::cmdFen(StringArguments& arguments)
{

}

//UCI commands
void CommandEngine::cmdGo(StringArguments& arguments)
{

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

}