#include "EngineAB.h"

EngineAlphaBeta::EngineAlphaBeta()
{

}

EngineAlphaBeta::~EngineAlphaBeta()
{
}

/// @brief Alpha Beta prototype search
void EngineAlphaBeta::search()
{
    std::cout << "Searching!" << std::endl;
    principal_variation.clear();
    Move move;
    move.setTo(16);
    move.setFrom(8);
    //move.setUpgrade(5);
    eval();
    principal_variation.push_back(move);
}

/// @brief Evaluation function. Returns the score of the board in millipawns
void EngineAlphaBeta::eval()
{
    const int PAWN_VALUE = 1000;
    const int BISHOP_VALUE = 3000;
    int score = 0;
    //0: empty, 1: wPawn, 2: wKnight, 3:wBishop, 4: wRook, 5: wQueen, 6: wKing, 7: bPawn, 8: bKnight, 9: bBishop, 10: bRook, 11: bQueen, 12: bKing
    u64 test = board.bitboard().pieces[1];
    u8 pop = populationCount(test);
    std::cout << test << std::endl;
}

/// @brief Naive min-max implementation
void EngineAlphaBeta::negamax()
{

}