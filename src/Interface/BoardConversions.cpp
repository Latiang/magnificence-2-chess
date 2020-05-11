#include "BoardConversions.h"

/// @brief This function converts a Bitboard class to a prettified string notation used for testing
std::string BoardConversions::bbToDisplayString(BitBoard& board)
{
    std::string board_string = "\n";
	int c;
	for (int y = 9; y >= 0; y--)
	{
		for (int x = -2; x < 8; x++)
		{
			if (y == 9 && x < 0)
				board_string += " ";
			else if (y == 9)
				board_string += char(65 + x);
			else if (y == 8)
				board_string += " ";
			else if (x == -2)
				board_string += std::to_string(y + 1);
			else if (x == -1)
				board_string += " ";
			else
			{
				c = (y*8) + x;
				board_string += pieceToChar(board.mailboard().pieces[c]);
			}
		}
		board_string += "\n";
	}
	board_string += "\n";
	return board_string;
}

/// @brief This function converts a piece (int) to a representative character, used in the displayString function
char BoardConversions::pieceToChar(int piece)
{
    switch (piece)
	{
	case 0:
		return '.'; //Nothing
	case 1: //White pieces
		return 'P';
	case 2:
		return 'N';
	case 3:
		return 'B';
	case 4:
		return 'R';
	case 5:
		return 'Q';
	case 6:
		return 'K';
	case 7: //Black pieces
		return 'p';
	case 8:
		return 'n';
	case 9:
		return 'b';
	case 10:
		return 'r';
	case 11:
		return 'q';
	case 12:
		return 'k';
	}
	return 0;
}

/// @brief This function converts a Bitboard class to the FEN standard notation (string)
/// FEN example (starting position): rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
std::string BoardConversions::bbToFenString(BitBoard& board)
{

}

/// @brief This function converts a Move struct to Long Algebraic Move Notation (string, ex 'a2a3')
std::string BoardConversions::moveToAlgebaricMove(Move& move)
{
    std::string alg_move;
    int from = move.from();
	int to = move.to();
	alg_move += from % 8 + 'a';
	alg_move += from / 8 + '1';
	alg_move += to % 8 + 'a';
	alg_move += to / 8 + '1';
	//PAWN PROMOTIONS MISSING!!! eg a7a8q
	return alg_move;
}

/// @brief This function converts a string of Long Algebraic Move Notation (ex 'a2a3') to a Move Struct
Move BoardConversions::algebraicMoveToMove(std::string alg_move)
{
    int a = (alg_move[1] - '1') * 8 + (alg_move[0] - 'a');
	int b = (alg_move[3] - '1') * 8 + (alg_move[2] - 'a');
    Move move;
    move.setFrom(a);
    move.setTo(b);
	//PAWN PROMOTIONS MISSING!!! eg a7a8q
	return move;
}