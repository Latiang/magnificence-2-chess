
#pragma once
#include <string>
#include "../Board/BitBoard.h"

class BoardConversions
{
public:
    static std::string bbToDisplayString(BitBoard& board);
    static std::string bbToFenString(BitBoard& board);
    static std::string moveToAlgebaricMove(Move& move);
    static Move algebraicMoveToMove(std::string algMove);
private:
    //Static class
    static char pieceToChar(int piece);

    BoardConversions() {};
    ~BoardConversions() {};
};
