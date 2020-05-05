#include "BoardConversions.h"

/// @brief This function converts a Bitboard class to a prettified string notation used for testing
std::string BoardConversions::bbToDisplayString(BitBoard& board)
{
    std::string boardString = "\n";
	int c = 63;
	for (size_t y = 0; y < 8; y++)
	{
		for (size_t x = 0; x < 8; x++)
		{
			boardString += pieceToChar(board.mailboard().pieces[c]);
			c--;
		}
		boardString += "\n";
	}
	boardString += "\n";
	return boardString;
}

/// @brief This function converts a piece (int) to a representative character, used in the displayString function
char BoardConversions::pieceToChar(int piece)
{
    switch (piece)
	{
	case 5: //White pieces
		return 'P';
	case 3:
		return 'R';
	case 4:
		return 'N';
	case 2:
		return 'B';
	case 1:
		return 'Q';
	case 0:
		return 'K';
	case 12: //Black pieces
		return 'p';
	case 10:
		return 'r';
	case 11:
		return 'n';
	case 9:
		return 'b';
	case 8:
		return 'q';
	case 7:
		return 'k';
	case 14: //Space
		return '.';
	}
	return 0;
}

/// @brief This function converts a Bitboard class to the FEN standard notation (string)
/// FEN example (starting position): rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
std::string BoardConversions::bbToFenString(BitBoard& board)
{

}

/// @brief This function converts a Move struct to Algebraic Move Notation (string, ex 'a2a3')
std::string BoardConversions::moveToAlgebaricMove(Move& move)
{
    std::string algMove;
    int from = move.from();
	int to = move.to();
	algMove += from % 8 + 'a';
	algMove += 8 - from / 8 + '0';
	algMove += to % 8 + 'a';
	algMove += 8 - to / 8 + '0';
	return algMove;
}

/// @brief This function converts a string of Algebraic Move Notation (ex 'a2a3') to a Move Struct
Move BoardConversions::algebraicMoveToMove(std::string algMove)
{
    int a = (algMove[1] - '0' - 1) * 8 + (7-(algMove[0] - 'a'));
	int b = ((algMove[3] - '0' - 1) * 8 + (7-(algMove[2] - 'a')));
    Move move;
    move.set_to(a);
    move.set_from(b);
	return move;
}