#include "EngineAB.h"

EngineAlphaBeta::EngineAlphaBeta()
{

}

EngineAlphaBeta::~EngineAlphaBeta()
{
}

/// @brief Very simple move sorting comparison
bool moveComp(Move& lhs, Move& rhs)
{
    return (lhs.getData() > rhs.getData());
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
    int alpha = -100000;
    int beta = 100000;
    while (moves_begin < moves_end)
    {   
        board.make(*moves_begin);
        //int score = -negamax(max_depth-1, moves_end);
        int score = -negamaxAB(alpha, beta, max_depth-1, moves_end);
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
int EngineAlphaBeta::eval(Move* moves_begin)
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

    Move* moves_end = board.moveGenWhite(moves_begin);
    score += (moves_end - moves_begin) * 10;

    moves_end = board.moveGenBlack(moves_begin);
    score += (moves_end - moves_begin) * -10;


    return score * board.toMove() + -score * !board.toMove();
}

/// @brief Naive min-max implementation
int EngineAlphaBeta::negamax(int depth, Move* moves_begin)
{
    if ( depth == 0 ) return eval(moves_begin);
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

/// @brief Naive min-max alpha beta implementation with quiescence
int EngineAlphaBeta::negamaxAB(int alpha, int beta, int depth, Move* moves_begin)
{
    if ( depth == 0 ) return quiescence(alpha, beta, moves_begin);

    Move* moves_end = board.moveGen(moves_begin);
    //Move sorting
    std::sort(moves_begin, moves_end, moveComp);
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

/// @brief Simple quiescence search. Evaluates the moves at the end of the branch until it becomes silent, ie no more captures.
int EngineAlphaBeta::quiescence(int alpha, int beta, Move* moves_begin)
{
    int stand_pat = eval(moves_begin);
    if( stand_pat >= beta )
        return beta;
    if( alpha < stand_pat )
        alpha = stand_pat;

    int score;
    Move* moves_end = board.moveGen(moves_begin);
    std::sort(moves_begin, moves_end, moveComp);
    while (moves_begin < moves_end)
    {
        if ((*moves_begin).isCapture())
        {
            board.make(*moves_begin);
            score = -quiescence( -beta, -alpha, moves_end);
            board.unmake(*moves_begin);

            if( score >= beta )
                return beta;
            if( score > alpha )
            alpha = score;
        }
        moves_begin++;
    }
    return alpha;
}