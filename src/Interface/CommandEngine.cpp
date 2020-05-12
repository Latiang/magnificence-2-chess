#include "CommandEngine.h"

CommandEngine::CommandEngine()
{
    main_engine.board = BitBoard(STARTPOS_FEN);
    side_engine.board = BitBoard(STARTPOS_FEN);
}

CommandEngine::~CommandEngine()
{
}

//Debug commands functions

/// @brief display. Display the board prettily in the console
void CommandEngine::cmdDisplay(StringArguments& arguments)
{
    std::string board_string = BoardConversions::bbToDisplayString(main_engine.board);
    std::cout << board_string << std::endl;
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

    bool leaf_node_optimize = true;
    if (arguments.arguments.size() > 1)
        leaf_node_optimize = false;
    
    int depth = std::stoi(arguments.arguments[0]);
    std::string leaf_optimize_string = leaf_node_optimize ? " (LO ON)" : " (LO OFF)";
    std::cout << "Performing Perft of Depth " << depth << leaf_optimize_string << std::endl;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    u64 score;
    if (leaf_node_optimize)
        score = perftLeaf(main_engine.board, depth);
    else
        score = perft(main_engine.board, depth);
    

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    float time_taken =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() * 0.001;
    float mnodes_per_second = score/(time_taken * 1000000);
    std::cout << "Perft result: " << std::to_string(score) << " in " << time_taken << "s [" << mnodes_per_second << "/MNPS]" << std::endl;
}

/// @brief cmd: fen. Outputs the board as a fen string
void CommandEngine::cmdFen(StringArguments& arguments)
{
    std::string fen_string = BoardConversions::bbToFenString(main_engine.board);
    std::cout << fen_string << std::endl;
}

/// @brief cmd: selfplay [time] [depth]. Runs a selfplay match between mainEngine and sideEngine
void CommandEngine::cmdSelfPlay(StringArguments& arguments)
{
    //Prototype function, does actually not work with current BitBoard implementation
    bool color_turn = WHITE; //White true, black false
    bool ply = 0;
    bool win = false;
    Move move;
    main_engine.color = WHITE; //White
    side_engine.color = BLACK; //Black
    bool color_win;
    main_engine.max_depth = 5;
    side_engine.max_depth = 5;
    while (!win)
    {
        if (color_turn == main_engine.color)
        {
            main_engine.search();
            move = main_engine.principal_variation[0];
        }
        else
        {
            side_engine.search();
            move = side_engine.principal_variation[0];
        }
        side_engine.board.make(move);
        main_engine.board.make(move);

        std::cout << "Turn complete" << std::endl;
        cmdDisplay(arguments);
        color_turn = !color_turn;
        if (main_engine.board.isWhiteInMate()) //Check if white king has been taken
        {
            color_win = BLACK;
            win = true;
        }
        else if (main_engine.board.isBlackInMate()) //Check if black king has been taken
        {
            color_win = WHITE;
            win = true;
        }
    }
    std::string win_color = color_win ? "White" : "Black";
    std::cout << win_color << " won the game!" << std::endl;
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
    if (currently_searching)
    {
        errorMessage("The engine is already searching");
        return;
    }

    main_engine.max_depth = arguments.getNamedArgument("depth", 100);
    main_engine.white_time = arguments.getNamedArgument("wtime", 100000);
    main_engine.black_time  = arguments.getNamedArgument("btime", 1000000);
    main_engine.white_time_increment = arguments.getNamedArgument("winc", 0);
    main_engine.black_time_increment = arguments.getNamedArgument("binc", 0);
    main_engine.move_time = arguments.getNamedArgument("movetime", -1);
    main_engine.max_nodes = arguments.getNamedArgument("nodes", -1);

    if (arguments.arguments.size() == 1 && arguments.arguments[0] != "infinite")
        main_engine.max_depth = std::stoi(arguments.arguments[0]);
    
    currently_searching = true;
    std::thread search_thread(&CommandEngine::runSearch, this, std::ref(main_engine));
    search_thread.detach();

}

/// @brief cmd: uci. Enter UCI mode
void CommandEngine::cmdUCI(StringArguments& arguments)
{
    interface_mode = UCI;
    std::cout << "id name " << ENGINE_NAME << std::endl;
	std::cout << "id author " << AUTHOR_NAME << std::endl;
	std::cout << "uciok" << std::endl;

}

/// @brief cmd: position <FEN/startpos> moves [move1] [move2] .... 
/// UCI standard. Sets the position of the board based on startpos/a fen string and then performs the sequence of moves supplied
void CommandEngine::cmdPosition(StringArguments& arguments)
{
    //Prototype function, not tested with a working Bitboard
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;

    std::string fen;

    if (arguments.arguments[0] == "startpos" || arguments.arguments[0] == "sp") //First argument is startpos, set board to starting position
        fen = STARTPOS_FEN;
    else
    {
        if (arguments.arguments[0] == "fen")
        {
            fen = arguments.isolateFenString(1);
        }
        else
        {
            fen = arguments.isolateFenString(0);
        }
    }
    
    int moves_begin_index = -1;
    for (size_t i = 0; i < arguments.arguments.size(); i++)
    {
        if (arguments.arguments[i] == "moves")
        {
            moves_begin_index = i+1;
            break;
        }
    }

    //mainEngine.board.bitboard.setFromFen(fen)
    main_engine.board = BitBoard(fen);
    main_engine.current_ply = 0;

    main_engine.color = main_engine.board.toMove();

    if (moves_begin_index == -1)
        return;
    for (size_t i = moves_begin_index; i < arguments.arguments.size(); i++)
    {
        //std::cout << arguments.arguments[i] << std::endl;
        Move move = BoardConversions::algebraicMoveToMove(arguments.arguments[i]);
        main_engine.board.make(move);
        main_engine.current_ply += 1;
    }
    main_engine.color = main_engine.board.toMove();
}

/// @brief cmd: stop. Stop the engine search
void CommandEngine::cmdStop(StringArguments& arguments)
{
    main_engine.stop_searching = true;
}

/// @brief cmd: isready. Replies with readyok if engine is ready for new commands
void CommandEngine::cmdIsReady(StringArguments& arguments)
{
    std::cout << "readyok" << std::endl;   
}

/// @brief: cmd divide. Perft score per each legal move of current position. Useful for debugging movegen
void CommandEngine::cmdDivide(StringArguments& arguments)
{
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;

    bool leaf_node_optimize = true;
    if (arguments.arguments.size() > 1)
        leaf_node_optimize = false;
    
    int depth = std::stoi(arguments.arguments[0]);

    std::string leaf_optimize_string = leaf_node_optimize ? " (LO ON)" : " (LO OFF)";
    std::cout << "Performing Perft Divide of Depth " << depth << leaf_optimize_string << std::endl;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    Move moves[100];
    Move* moves_start = moves;
    Move* moves_end = main_engine.board.moveGen(moves_start);
    
    int counter = 0;
    u64 total = 0;
    while (moves_start < moves_end)
    {
        main_engine.board.make(*moves_start);
        u64 perft_score;
        if (leaf_node_optimize)
            perft_score = perftLeaf(main_engine.board, depth-1);
        else
            perft_score = perft(main_engine.board, depth-1);
        total += perft_score;
        main_engine.board.unmake(*moves_start);
        std::string alg_move = BoardConversions::moveToAlgebaricMove(*moves_start);
        std::cout << alg_move << ": " << perft_score << std::endl;
        moves_start++;
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    float time_taken =  std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() * 0.001;
    float mnodes_per_second = total/(time_taken * 1000000);
    std::cout << "Total: " << std::to_string(total) << " in " << time_taken << "s [" << mnodes_per_second << "/MNPS]" << std::endl;
    
}

/// @brief: cmd move <move>. Move an algebraic move.
void CommandEngine::cmdMove(StringArguments& arguments)
{
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;

    std::string alg_move = arguments.arguments[0];
    Move move = BoardConversions::algebraicMoveToMove(alg_move);
    main_engine.board.make(move);
    main_engine.color = main_engine.board.toMove();
}

void CommandEngine::cmdUnmove(StringArguments& arguments)
{
    if (!areArgumentsCorreclyFormatted(arguments, 1))
        return;

    std::string alg_move = arguments.arguments[0];
    Move move = BoardConversions::algebraicMoveToMove(alg_move);
    main_engine.board.unmake(move);
    main_engine.color = main_engine.board.toMove();
}

/// @brief: cmd moves. List the legal moves
void CommandEngine::cmdLegalMoves(StringArguments& arguments)
{
    Move moves[100];
    Move* moves_start = moves;
    Move* moves_end = main_engine.board.moveGen(moves_start);

    std::string moves_str = "";
    int count = moves_end - moves_start;
    while (moves_start < moves_end)
    {
        std::string alg_move = BoardConversions::moveToAlgebaricMove(*moves_start);
        moves_str += alg_move + "\n";
        moves_start++;
    }
    std::cout << count << " Legal moves" << std::endl;
    if (moves_str != "")
        std::cout << moves_str << std::endl;
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
    if (interface_mode == TESTING)
        std::cout << "The command failed due to: " << message << std::endl;
}

/// @brief Start the search in an engine. This function is called from a separate thread as to keep the program responsive
void CommandEngine::runSearch(Engine& engine)
{ 
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    engine.search();

    currently_searching = false;

    Move best_move = engine.principal_variation[0];
    std::string best_move_alg = BoardConversions::moveToAlgebaricMove(best_move, engine.color);

    //UCI response
    std::cout << "bestmove " << best_move_alg << std::endl;

    if (interface_mode == TESTING)
        std::cout << "mgnf2: ";
}