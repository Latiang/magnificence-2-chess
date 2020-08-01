#include "EngineMCT.h"



int base_score = 0;
const double EXPLORATION=1.4;


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

    const int MOVE_VALUE = 50;
    if (board.toMove()) {
        score +=num_moves * MOVE_VALUE;
        Move* moves_end = board.moveGenBlack(moves_begin);
        score -= (moves_end - moves_begin) * MOVE_VALUE;
    }
    else {
        score -= num_moves * MOVE_VALUE;
        Move* moves_end = board.moveGenWhite(moves_begin);
        score += (moves_end - moves_begin) * MOVE_VALUE;
    }


    return score * board.toMove() + -score * !board.toMove();
}
#if defined(DEBUG)
bool problem = false;
#endif
/// @brief Simple quiescence search. Evaluates the moves at the end of the branch until it becomes silent, ie no more captures.
int quiescence(int alpha, int beta, Move* moves_begin, BitBoard &board)
{
    Move *moves_end = board.moveGen(moves_begin);
    if (moves_end == moves_begin) {
        if (moves_begin->from() == 1) {
            return 0;
        }
        else {
            return -10000000;
        }
    }
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
            #if defined(DEBUG)
            if (moves_begin->taken() == 6) {
                return 1<<30;
            }
            #endif
            board.make(*moves_begin);
            score = -quiescence( -beta, -alpha, moves_end, board);
            board.unmake(*moves_begin);
            #if defined(DEBUG)
            if (score == -1073741824) {
                std::string pos = "";
                pos = BoardConversions::bbToFenString(board);
                std::cout << pos;
                return 5;
            }
            #endif
            if( score >= beta )
                return beta;
            if( score > alpha )
            alpha = score;
        }
        moves_begin++;
    }
    return alpha;
}


/// @brief Naive min-max alpha beta implementation with quiescence
int negamaxAB(int alpha, int beta, int depth, Move* moves_begin, BitBoard& board)
{
    if ( depth <= 0 ) return quiescence(alpha, beta, moves_begin, board);

    Move* moves_end = board.moveGen(moves_begin);
    if (moves_end == moves_begin) {
        if (moves_begin->from() == 1){
            return 0;
        }
        else {
            return -1000000;
        }
    }
    //Move sorting
    std::sort(moves_begin, moves_end, moveCompLocal);
    while (moves_begin < moves_end)
    {
        #if defined(DEBUG)
        if (moves_begin->taken() == 6) {
            problem = true;
            return 0;
        }
        #endif
        board.make(*moves_begin);
        int score = -negamaxAB( -beta, -alpha, depth - 1, moves_end, board);
        board.unmake(*moves_begin);
        #if defined(DEBUG)
        if (problem) {
            std::cout << BoardConversions::bbToFenString(board) << std::endl;
            return 0;
        }
        #endif
        if( score >= beta )
            return beta;   //  fail hard beta-cutoff
        alpha = std::max(alpha, score);
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
    alfa = -negamaxAB(-beta, -alfa, 1, move_end, board);
    board.unmake(best_move);
    auto start_fen = BoardConversions::bbToFenString(board);
    while (move_start != move_end) {
        board.make(*move_start);
        int score = -negamaxAB(-beta, -alfa, 1, move_end, board);
        board.unmake(*move_start);
        #if defined(DEBUG)
        if (problem) {
            std::cout << "Start fen: " << start_fen << "\nEnd fen:   " << BoardConversions::bbToFenString(board) << std::endl;
        }
        #endif
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
    auto start = std::chrono::high_resolution_clock::now();
    time_t left;
    base_score = negamaxAB(-100000000, 100000000, 3, move_space, board);
    if (board.toMove()) {
        left = white_time;
    }
    else {
        left = black_time;
    }
    double time_allowed = left / 20000.0;
    std::cout << "Searching..." << std::endl;
    principal_variation.clear();
    Move move;
    MCTNode root(move);
    expandTree(root);
    std::chrono::duration<double> passed = std::chrono::high_resolution_clock::now() - start;
    while (passed.count() < time_allowed) {
        for (size_t i = 0; i < 100; i++)
        {
            searchTree(root);
        }
        passed = std::chrono::high_resolution_clock::now() - start;
    }
    u64 num = root.children[0].visits;
    move = root.children[0].move;
    double score = root.children[0].score;
    for (size_t i = 1; i < root.children.size(); i++)
    {
        double temp_score = root.children[i].score;
        if (temp_score >= score) {
            score = root.children[i].score;
            move = root.children[i].move;
            num = root.children[i].visits;
        }
    }
    std::cout << "Win chance: " << (int)(((score * 100) / num)) << "%\n";
    std::cout << score << "   " << num << "\n";
    principal_variation.push_back(move);
}

Winner EngineMCT::simulateGame() {
    Winner winner;
    bool current_player = board.toMove();
    Move *start = move_space;
    while (true) {
        Move *end = board.moveGen(start);
        if (end == start) {
            if (start->from() == 1) {
                winner =  Winner::D;
            }
            else if (board.toMove()) {
                winner = Winner::B;
            }
            else {
                winner = Winner::W;
            }
            break;
        }
        if (board.getSilent() >= 50) {
            winner = Winner::D;
            break;
        }
        move_storage.push_back(playout_policy(start, end, board));
        board.make(move_storage.back());
    }

    while (move_storage.size()) {
        board.unmake(move_storage.back());
        move_storage.pop_back();
    }
    return winner;
}

void EngineMCT::expandTree(MCTNode &node) {
    Move *start = move_space;
    Move *end = board.moveGen(start);
    std::sort(start, end, moveCompLocal);
    if (end == start) {
        if (end->from() == 1) {
            node.addScore(0.5);
        }
        else {
            node.addScore(0);
        }
        return;
    }
    if (board.getSilent() >= 50) {
        node.addScore(0.5);
        return;
    }
    node.addChildren(start, end);
    Move best = playout_policy(start, end, board);
    for (MCTNode &curr_node : node.children) {
        board.make(curr_node.move);
        curr_node.value = scorePosition();
        board.unmake(curr_node.move);
    }
    node.sortChildren();
    node.addScore(node.value);
    return;
}

void EngineMCT::searchTree(MCTNode & node) {
    if (board.getSilent() >= 51) {
        node.addScore(0.5);
        return;
    }
    if (node.children.size() == 0) {
        expandTree(node);
        return;
    }
    double Nlog = std::log(std::max((double)node.visits, 0.0));
    double best_score = -1;
    MCTNode *best_node;
    for (MCTNode &curr_node: node.children) {
        double wins = curr_node.score;
        double n = std::max((double)curr_node.visits, 1.0);
        double score = wins / n + EXPLORATION*sqrtf64(Nlog/n);
        if (score > best_score) {
            best_node = &curr_node;
            best_score = score;
        }
    }
    board.make(best_node->move);
    searchTree(*best_node);
    board.unmake(best_node->move);
}

double EngineMCT::scorePosition() {
    //scores with respect to oponent in current position due to wanting to score for person above
    int score = negamaxAB(-100000000, 100000000, 1, move_space, board);
    return 1/(1 + std::exp((score - base_score) / 1000)); //we only look at a change in score in order to facilitate better play 
}