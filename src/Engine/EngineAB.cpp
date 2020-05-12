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
    std::cout << "Searching..." << std::endl;
    principal_variation.clear();
    Move moves[10000];
    Move* moves_begin = moves;
    Move* moves_end = board.moveGen(moves);
    int max = -100000;
    Move* best_move;
    while (moves_begin < moves_end)
    {   
        board.make(*moves_begin);
        int score = -negamax(max_depth-1, moves_end);
        board.unmake(*moves_begin);
        
        if (score > max)
        {
            best_move = moves_begin;
            max = score;
        }
        moves_begin++;
    }
    std::cout << "Score: " << max << std::endl;
    principal_variation.push_back(*best_move);
}

/// @brief Evaluation function. Returns the score of the board in millipawns
int EngineAlphaBeta::eval()
{
    const int PAWN_VALUE = 1000;
    const int BISHOP_VALUE = 3000;
    const int KNIGHT_VALUE = 3000;
    const int ROOK_VALUE = 5000;
    const int QUEEN_VALUE = 9000;
    const int KING_VALUE = 100000;
    u64 const *pieces = board.bitboard().pieces;
    //0: empty, 1: wPawn, 2: wKnight, 3:wBishop, 4: wRook, 5: wQueen, 6: wKing, 7: bPawn, 8: bKnight, 9: bBishop, 10: bRook, 11: bQueen, 12: bKing
    int score = populationCount(pieces[1]) * PAWN_VALUE +  populationCount(pieces[2]) * KNIGHT_VALUE + populationCount(pieces[3]) * BISHOP_VALUE + 
    populationCount(pieces[4]) * ROOK_VALUE + populationCount(pieces[5]) * QUEEN_VALUE + populationCount(pieces[6]) * KING_VALUE +
        populationCount(pieces[7]) * -PAWN_VALUE + populationCount(pieces[8]) * -KNIGHT_VALUE + populationCount(pieces[9]) * -BISHOP_VALUE + 
        populationCount(pieces[10]) * -ROOK_VALUE + populationCount(pieces[11]) * -QUEEN_VALUE + populationCount(pieces[12]) * -KING_VALUE;
    return score * board.toMove() + -score * !board.toMove();
}

/// @brief Naive min-max implementation
int EngineAlphaBeta::negamax(int depth, Move* moves_begin)
{
    if ( depth == 0 ) return eval();
    int max = -1000000;
    Move* moves_end = board.moveGen(moves_begin);
    while (moves_begin < moves_end)
    {
        board.make(*moves_begin);
        int score = -negamax(depth - 1, moves_end);
        board.unmake(*moves_begin);
        max = std::max(score, max);
        moves_begin++;
    }
    return max;
}

/// @brief Naive min-max alpha beta implementation
int EngineAlphaBeta::negamaxAB(int alpha, int beta, int depth, Move* moves_begin)
{
    if ( depth == 0 ) return eval();

    Move* moves_end = board.moveGen(moves_begin);
    while (moves_begin < moves_end)
    {
        board.make(*moves_begin);
        int score = -negamaxAB( -beta, -alpha, depth - 1, moves_end);
        board.unmake(*moves_begin);
        if( score >= beta )
            return beta;   //  fail hard beta-cutoff
        alpha = std::max(alpha, score);
        moves_begin++;
    }
    return alpha;
}