#include "BoardConversions.h"

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

std::string BoardConversions::bbToFenString(BitBoard& board)
{

}

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

Move BoardConversions::algebraicMoveToMove(std::string algMove)
{
    int a = (algMove[1] - '0' - 1) * 8 + (7-(algMove[0] - 'a'));
	int b = ((algMove[3] - '0' - 1) * 8 + (7-(algMove[2] - 'a')));
    Move move;
    move.set_to(a);
    move.set_from(b);
	return move;
}