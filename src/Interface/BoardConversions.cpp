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
std::string BoardConversions::bbToFenString(const BitBoard& board)
{
	std::string fen_string;
	int empty_counter = 0;
	//Positions
	for (int y = 7; y > -1; y--)
	{
		for (int x = 0; x < 8; x++)
		{
			int piece = board.mailboard().pieces[y * 8 + x];
			if (piece == 0){
				empty_counter++;
				continue;
			}
			else if (empty_counter != 0)
			{
				fen_string += std::to_string(empty_counter);
				empty_counter = 0;
			}
			switch (board.mailboard().pieces[y * 8 + x])
			{
            case 1: //White pieces
                fen_string += 'P';
                break;
            case 2:
                fen_string += 'N';
                break;
            case 3:
                fen_string += 'B';
                break;
            case 4:
                fen_string += 'R';
                break;
            case 5:
                fen_string +='Q';
                break;
            case 6:
                fen_string += 'K';
                break;
            case 7: //Black pieces
                fen_string += 'p';
                break;
            case 8:
                fen_string += 'n';
                break;
            case 9:
                fen_string += 'b';
                break;
            case 10:
                fen_string += 'r';
                break;
            case 11:
                fen_string += 'q';
                break;
            case 12:
                fen_string += 'k';
                break;
            }
		}
		if (empty_counter != 0)
		{
			fen_string += std::to_string(empty_counter);
			empty_counter = 0;
		}
		if (y != 0)
			fen_string += '/';
	}
	if (empty_counter != 0)
	{
		fen_string += std::to_string(empty_counter);
		empty_counter = 0;
	}
	fen_string += " ";
	//Color turn
	fen_string += board.toMove() ? 'w' : 'b';
	fen_string += " ";
	u8 castling = board.getCastling();
	bool castling_white_kingside = castling & (1 << 3);
	bool castling_white_queenside = castling & (1 << 2);
	bool castling_black_kingside = castling & (1 << 1);
	bool castling_black_queenside = castling & (1 << 0);
	//Castling
	if (!castling_white_kingside && !castling_white_queenside
		&& !castling_black_kingside && !castling_black_queenside)
		fen_string += "-";
	else
	{
		fen_string += (castling_white_kingside ? "K" : "");
		fen_string += (castling_white_queenside ? "Q" : "");
		fen_string += (castling_black_kingside ? "k" : "");
		fen_string += (castling_black_queenside ? "q" : "");
	}
	fen_string += " ";
	//En passant
	if (board.getEP() == 8) //No en passant square, column is outside board
		fen_string += "-";
	else //Valid en passant square
	{
		//x3 for white, x6 for black
		std::string pos = "";
		pos += 'a' + board.getEP();
		pos += (board.toMove() == WHITE ? '6' : '3');
		fen_string += pos;
	}
	
	//Turns
	fen_string += " 1 0"; 
	//fenString += " " + to_string((board.totalPly + 1) / 2);
	return fen_string;
}

/// @brief This function converts a Move struct to Long Algebraic Move Notation (string, ex 'a2a3')
std::string BoardConversions::moveToAlgebaricMove(Move& move, bool color_to_move)
{
    std::string alg_move;
    int from = move.from();
	int to = move.to();
	alg_move += from % 8 + 'a';
	alg_move += from / 8 + '1';
	alg_move += to % 8 + 'a';
	alg_move += to / 8 + '1';
	if (move.upgrade() != 0) //Promotion information
		alg_move += pieceToChar(move.upgrade() + 6 * !color_to_move);

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
	if (alg_move.size() == 5) //Promotion information
		//Extract promotion type
		switch (alg_move[4])
		{
			case 'n':
			case 'N':
				move.setUpgrade(2);
				break;
			case 'b':
			case 'B':
				move.setUpgrade(3);
				break;
			case 'R':
			case 'r':
				move.setUpgrade(4);
				break;
			case 'Q':
			case 'q':
				move.setUpgrade(5);
				break;
		}
	//move.setUpgrade()
	//PAWN PROMOTIONS MISSING!!! eg a7a8q
	return move;
}