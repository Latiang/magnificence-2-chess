#include "CommandEngine.h"

CommandEngine::CommandEngine()
{

}

CommandEngine::~CommandEngine()
{
}

//Debug commands functions

/// @brief display. Display the board prettily in the console
void CommandEngine::cmdDisplay(StringArguments& arguments)
{
    std::string boardString = BoardConversions::bbToDisplayString(mainEngine.board);
    std::cout << boardString << std::endl;
}

/// @brief cmd: help. Display a list of commands
void CommandEngine::cmdHelp(StringArguments& arguments)
{
    std::cout << HELP_STRING << std::endl;
}

/// @brief cmd: quit. Exit the program
void CommandEngine::cmdQuit(StringArguments& arguments)
{
    std::cout << "Exiting Magnficence 2..." << std::endl;
    exit = true;
}

/// @brief cmd: perft <depth>. Does a <depth> deep counting of leaf nodes in the legal moves tree
void CommandEngine::cmdPerft(StringArguments& arguments)
{
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;
    
    int depth = std::stoi(arguments.arguments[0]);
    std::cout << "Performing Perft " << depth << std::endl;
    perft(mainEngine.board, depth);

}

/// @brief cmd: fen. Outputs the board as a fen string
void CommandEngine::cmdFen(StringArguments& arguments)
{
    std::string fenString = mainEngine.board.fen_string();
    std::cout << fenString << std::endl;
}

/// @brief cmd: selfplay [time] [depth]. Runs a selfplay match between mainEngine and sideEngine
void CommandEngine::cmdSelfPlay(StringArguments& arguments)
{
    //Prototype function, does actually not work with current BitBoard implementation
    bool colorTurn = true; //White true, black false
    bool ply = 0;
    bool win = false;
    Move move;
    mainEngine.color = true; //White
    sideEngine.color = false; //Black
    while (!win)
    {
        if (colorTurn == mainEngine.color)
        {
            mainEngine.search();
            move = mainEngine.principalVariation[0];
        }
        else
        {
            sideEngine.search();
            move = sideEngine.principalVariation[0];
        }
        sideEngine.board.make(move);
        mainEngine.board.make(move);

        colorTurn = !colorTurn;
        //if mainEngine.winState()
            //win = true
    }
}

// UCI commands functions

/**
 * @brief 
 * cmd: go. Starts searching in the engine on a separate thread
 *  Received command in this format: "go wtime 300000 btime 300000 winc 0 binc 0"
 * Also allowed: "go <maxdepth>"
 * Output format: "bestmove h7h5"
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

/// @brief cmd: uci. Enter UCI mode
void CommandEngine::cmdUCI(StringArguments& arguments)
{
    interfaceMode = UCI;
    std::cout << "id name " << ENGINE_NAME << std::endl;
	std::cout << "id author " << AUTHOR_NAME << std::endl;
	std::cout << "uciok" << std::endl;

}

/// @brief cmd: position <FEN/startpos> [move1] [move2] .... 
/// UCI standard. Sets the position of the board based on startpos/a fen string and then performs the sequence of moves supplied
void CommandEngine::cmdPosition(StringArguments& arguments)
{
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;

    int movesBeginIndex = 1;
    std::string fen;

    if (arguments.arguments[0] == "startpos") //First argument is startpos, set board to starting position
        fen = STARTPOS_FEN;
    else
    {
        if (!areArgumentsCorreclyFormatted(arguments, 6))
            return;

        if (arguments.arguments[0] == "fen")
        {
            fen = arguments.isolateFenString(1);
            movesBeginIndex = 7;
        }
        else
        {
            fen = arguments.isolateFenString(0);
            movesBeginIndex = 6;
        }
    }
    
    //mainEngine.board.bitboard.setFromFen(fen)

    for (size_t i = movesBeginIndex; i < arguments.arguments.size(); i++)
    {
        Move move = BoardConversions::algebraicMoveToMove(arguments.arguments[i]);
        mainEngine.board.make(move);
    }
    


}

/// @brief cmd: stop. Stop the engine search
void CommandEngine::cmdStop(StringArguments& arguments)
{
    mainEngine.stopSearching = true;
}

/// @brief cmd: isready. Replies with readyok if engine is ready for new commands
void CommandEngine::cmdIsReady(StringArguments& arguments)
{
    std::cout << "readyok" << std::endl;   
}

/// @brief: cmd divide. Perft score per each legal move of current position. Useful for debugging movegen
void CommandEngine::cmdDivide(StringArguments& arguments)
{

}

/// @brief: cmd move <move>. Move an algebraic move.
void CommandEngine::cmdMove(StringArguments& arguments)
{
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;

    std::string algMove = arguments.arguments[0];
    Move move = BoardConversions::algebraicMoveToMove(algMove);
    std::cout << "Move: " << move.from() << " " << move.to() << std::endl;
}

/// @brief: cmd moves. List the legal moves
void CommandEngine::cmdLegalMoves(StringArguments& arguments)
{

}


//Helper functions

/// @brief Checks that a command is correctly formatted by checking the number of arguments equals the given number
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

/// @brief Sends an error message related to a command. It is only sent in Testing mode, UCI does not accept other output like this
void CommandEngine::errorMessage(std::string message)
{
    if (interfaceMode == TESTING)
        std::cout << "The command failed due to: " << message << std::endl;
}

/// @brief Start the search in an engine. This function is called from a separate thread as to keep the program responsive
void CommandEngine::runSearch(Engine& engine)
{ 
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    engine.search();

    currentlySearching = false;

    Move bestMove = engine.principalVariation[0];
    std::string bestMoveAlg = BoardConversions::moveToAlgebaricMove(bestMove);

    //UCI response
    std::cout << "bestmove " << bestMoveAlg << std::endl;

    if (interfaceMode == TESTING)
        std::cout << "mgnf2: ";
}