
/**
 * @file BoardConversions.h
 * @author William Sandström and Harald Bjurulf
 * @brief This file contains the static class BoardConversions
 * @version 0.1
 * @date 2020-05-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#include "../Board/BitBoard.h"
#include <string>
#include <iostream>

/**
 * @brief This is a static helper class which is used for various Board state conversions and also other types of conversions which are needed.
 * @details Examples are the Bitboard class to FEN strings (a standardized of representing a chess board with a string), the Move struct to algebraic string form etc.
 */
class BoardConversions
{
public:
    static std::string bbToDisplayString(BitBoard& board);
    static std::string bbToFenString(const BitBoard& board);
    static std::string moveToAlgebaricMove(Move& move, bool color_to_move=WHITE);
    static Move algebraicMoveToMove(std::string alg_move);
private:
    static char pieceToChar(int piece);

    //Static class, constructors private
    BoardConversions() {};
    ~BoardConversions() {};
};
