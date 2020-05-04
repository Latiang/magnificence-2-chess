#include "CommandEngine.h"

CommandEngine::CommandEngine(/* args */)
{
    //std::atomic<int> test{1};
}

CommandEngine::~CommandEngine()
{
}

//Debug commands

// @brief display. Display the board prettily in the console
void CommandEngine::cmdDisplay(StringArguments& arguments)
{
    std::string boardString = StringHelpers::getDisplayString(mainEngine.board);
    std::cout << boardString << std::endl;
}

// @brief cmd: help. Display a list of commands
void CommandEngine::cmdHelp(StringArguments& arguments)
{
    std::cout << HELP_STRING << std::endl;
}

// @brief cmd: quit. Exit the program
void CommandEngine::cmdQuit(StringArguments& arguments)
{
    std::cout << "Exiting Magnficence 2..." << std::endl;
    exit = true;
}

// @brief cmd: perft <depth>. Does a <depth> deep counting of leaf nodes in the legal moves tree
void CommandEngine::cmdPerft(StringArguments& arguments)
{
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;
    
    int depth = std::stoi(arguments.arguments[0]);
    std::cout << "Performing Perft " << depth << std::endl;
    perft(mainEngine.board, depth);

}

// @brief cmd: fen. Outputs the board as a fen string
void CommandEngine::cmdFen(StringArguments& arguments)
{
    std::string fenString = mainEngine.board.fen_string();
    std::cout << fenString << std::endl;
}

//UCI commands

/**
 * @brief 
 * //cmd: go. Starts searching in the engine on a separate thread
 * // Received command in this format: "go wtime 300000 btime 300000 winc 0 binc 0"
 * //Also allowed go <maxdepth>
 * //Output format: "bestmove h7h5"
 */
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

// @brief cmd: uci. Enter UCI mode
void CommandEngine::cmdUCI(StringArguments& arguments)
{
    interfaceMode = UCI;
    std::cout << "id name " << ENGINE_NAME << std::endl;
	std::cout << "id author " << AUTHOR_NAME << std::endl;
	std::cout << "uciok" << std::endl;

}

void CommandEngine::cmdPosition(StringArguments& arguments)
{

}

// @brief cmd: setboard. Set the board to a fen string
void CommandEngine::cmdSetBoard(StringArguments& arguments)
{
    std::string fenString = arguments.argumentsString;
    //mainEngine.board.setByFen(fenString)
}

// @brief cmd: stop. Stop the engine search
void CommandEngine::cmdStop(StringArguments& arguments)
{
    mainEngine.stopSearching = true;
}

// @brief cmd: isready. Replies with readyok if engine is ready for new commands
void CommandEngine::cmdIsReady(StringArguments& arguments)
{
    std::cout << "readyok" << std::endl;   
}

// @brief: cmd divide. Perft score per each legal move of current position. Useful for debugging movegen
void CommandEngine::cmdDivide(StringArguments& arguments)
{

}

// @brief: cmd move <move>. Move an algebraic move.
void CommandEngine::cmdMove(StringArguments& arguments)
{

}

// @brief: cmd moves. List the legal moves
void CommandEngine::cmdLegalMoves(StringArguments& arguments)
{

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