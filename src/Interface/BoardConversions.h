
#pragma once
#include <string>
#include "../Board/BitBoard.h"

class BoardConversions
{
public:
    static std::string bbToDisplayString(BitBoard& board);
    static std::string bbToFenString(BitBoard& board);
private:
    //Static class
    static char pieceToChar(int piece);

    BoardConversions() {};
    ~BoardConversions() {};
};
