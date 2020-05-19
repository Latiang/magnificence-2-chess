#include "EngineMCT.h"
/// @brief Very simple move sorting comparison
bool moveCompLocal(Move& lhs, Move& rhs)
{
    return (lhs.getData() > rhs.getData());
}


int eval(const BitBoard& board, size_t num_moves, Move *moves_begin)
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
    if (board.toMove()) {
        score +=num_moves * 100;
        Move* moves_end = board.moveGenBlack(moves_begin);
        score -= (moves_end - moves_begin) * 100;
    }
    else {
        score -= num_moves * 100;
        Move* moves_end = board.moveGenWhite(moves_begin);
        score += (moves_end - moves_begin) * 100;
    }


    return score * board.toMove() + -score * !board.toMove();
}


/// @brief Naive min-max alpha beta implementation with quiescence
int negamaxAB(int alpha, int beta, int depth, Move* moves_begin, BitBoard& board)
{
    if ( depth == 0 ) return quiescence(alpha, beta, moves_begin, board);

    Move* moves_end = board.moveGen(moves_begin);
    if (moves_end == moves_begin) {
        if (moves_begin->from() == 1){
            return 0;
        }
        else {
            return -100000000;
        }
    }
    //Move sorting
    std::sort(moves_begin, moves_end, moveCompLocal);
    while (moves_begin < moves_end)
    {
        board.make(*moves_begin);
        int score = -negamaxAB( -beta, -alpha, depth - 1, moves_end, board);
        board.unmake(*moves_begin);
        if( score >= beta )
            return beta;   //  fail hard beta-cutoff
        alpha = std::max(alpha, score);
        moves_begin++;
    }
    return alpha;
}

/// @brief Simple quiescence search. Evaluates the moves at the end of the branch until it becomes silent, ie no more captures.
int quiescence(int alpha, int beta, Move* moves_begin, BitBoard &board)
{
    Move *moves_end = board.moveGen(moves_begin);
    int stand_pat = eval(board, moves_end - moves_begin, moves_end);
    if( stand_pat >= beta )
        return beta;
    if( alpha < stand_pat )
        alpha = stand_pat;

    int score;
    std::sort(moves_begin, moves_end, moveCompLocal);
    while (moves_begin < moves_end)
    {
        if ((*moves_begin).isCapture())
        {
            board.make(*moves_begin);
            score = -quiescence( -beta, -alpha, moves_end, board);
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

Move alfaBeta1Policy(Move *move_start, Move *move_end, BitBoard& board) {
    Move best_move = *move_start;
    move_start++;
    int alfa = -100000000;
    int beta = 100000000;
    board.make(best_move);
    alfa = -quiescence(-beta, -alfa, move_end, board);
    board.unmake(best_move);
    while (move_start != move_end) {
        board.make(*move_start);
        int score = -quiescence(-beta, -alfa, move_end, board);
        board.unmake(*move_start);
        if (score > alfa) {
            alfa = score;
            best_move = *move_start;
        }
        move_start++;
    }
    return best_move;
}


EngineMCT::EngineMCT() {
    playout_policy = &alfaBeta1Policy;
}

EngineMCT::EngineMCT(Move (*playout_policy_in)(Move*, Move*, BitBoard&)) {
    playout_policy = playout_policy_in;
}


EngineMCT::~EngineMCT() {
}


/// @brief MCT prototype search
void EngineMCT::search()
{
    std::cout << "Searching..." << std::endl;
    principal_variation.clear();
    Move move;
    principal_variation.push_back(move);
}