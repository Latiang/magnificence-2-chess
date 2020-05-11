/**
 * @file BitBoard.cpp
 * @author Harald Bjurulf (haraldbjurulf@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "BitBoard.h"

bool has_initiated = false;
std::mt19937_64 rng(2348723472648796678);
u64 zoobrist_keys[64 * 12 + 8 + 4];
u64 king_masks[64];
u64 knight_masks[64];
std::vector<u64> bishop_masks[64];
std::vector<u64> rook_masks[64];

void init() {
    //initialise zoobrist keys
    for (size_t i = 0; i < 780; i++)
    {
        zoobrist_keys[i] = rng();
    }
    for (size_t i = 0; i < 64; i++)
    {
        //king
        king_masks[i] = 0;
        if (i % 8 != 0)
        {
            king_masks[i] |= ONE << (i - 1);
            if (i / 8 != 7) {
                king_masks[i] |= ONE << (i + 8 - 1);
            }
            if (i / 8 != 0) {
                king_masks[i] |= ONE << (i - 8 - 1);
            }
        }
        
    }
    
    
    
}

void initiate();

void BitBoard::addPiece(size_t index, u8 piece) {
    u8 removed = mailboard_var.pieces[index];
    mailboard_var.pieces[index] = piece; //Mail board updated

    bitboard_var.pieces[removed] &= ~(ONE << index);
    bitboard_var.pieces[piece] |= ONE << index;

    //Modify zoobrist hash

}

void BitBoard::removePiece(size_t index) {
    addPiece(index, 0);
}


/**
 * @brief Creates a new BitBoard in the startposition
 * 
 */
BitBoard::BitBoard() {
    (*this) =  BitBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

/**
 * @brief Copies the given bitboard
 * 
 * @param original 
 */
BitBoard::BitBoard(const BitBoard &original) {
    (*this) = original;
}

/**
 * @brief Creates a board from a fen_string
 * 
 * @param fen_string 
 */
BitBoard::BitBoard(const std::string &fen_string) {
    size_t index = 56;
    // Stora A 65, lilla a 97
    size_t i;
    for (i = 0; i < fen_string.size(); i++)
    {   
        if (fen_string[i] < 122 && fen_string[i] > 65) {
            u8 chr = fen_string[i];
            u8 piece;
            if (fen_string[i] > 96) {
                chr -= 32;

                piece = 6;
            }
            else {
                piece = 0;
            }
            switch (chr) {
                case 'P':
                    piece += 1;
                    break;
                case 'N':
                    piece += 2;
                    break;
                case 'B':
                    piece += 3;
                    break;
                case 'R':
                    piece += 4;
                    break;
                case 'Q':
                    piece += 5;
                    break;
                case 'K':
                    piece += 6;
                    break;
            }
            mailboard_var.pieces[index] = piece;
            index += 1;
            if (index % 8 == 0) {
                index -= 16;
            }
        }
        else if(fen_string[i] < 58 && fen_string[i] > 47) {
            u8 mem = fen_string[i] - 48;
            while (mem) {
                mailboard_var.pieces[index] = 0;
                index += 1;
                if (index % 8 == 0) {
                    index -= 16;
                }
                mem -= 1;
            }   
        }
        else if (fen_string[i] == '/') {
            continue;
        }
        else {
            break;
        }
    }
    // At this point fen_string[i] should be a ' '
    i += 1;
    if (fen_string[i] == 'w') {
        color = true;
    }
    else {
        color = false;
    }
    castling = 0;
    i += 2;
    while (fen_string[i] != ' ') {
        switch (fen_string[i])
        {
        case 'K':
            castling |= 0b1000;
            break;
        case 'Q':
            castling |= 0b0100;
            break;
        case 'k':
            castling |= 0b0010;
            break;
        case 'q':
            castling |= 0b0001;
            break;
        default:
            break;
        }
        i += 1;
    }
    i += 1;
    //now we do the EP
    if (fen_string[i] != '-') {
        ep = fen_string[i] - 97;
        i += 3;
    }
    else {
        ep = 8;
        i += 2;
    }
    silent = 0;
    while (i < fen_string.size() && fen_string[i] != ' ') {
        silent *= 10;
        silent += fen_string[i] - 48;
        i += 1;
    }
    zoobrist = 0;
}

void BitBoard::make(Move move) {
    silent_mem.push_back(silent);
    // Update castling
    if (castling & 0b1100) {
        if (move.from() == 7 || move.to() == 7) {
            castling &= 0b0111;
        }
        if (move.from() == 0 || move.to() == 0) {
            castling &= 0b1011;
        }
        if (move.to() == 4 || move.from() == 4) {
            castling &= 0b0011;
        }
    }
    if (castling & 0b0011) {
        if (move.from() == 63 || move.to() == 63) {
            castling &= 0b1101;
        }
        if (move.from() == 56 || move.to() == 56) {
            castling &= 0b1110;
        }
        if (move.to() == 60 || move.from() == 60) {
            castling &= 0b1100;
        }
    }

    u8 from = move.from();
    u8 to = move.to();
    u8 piece_moved = mailboard_var.pieces[move.from()];
    u8 color_mod = (color) * 6;

    //Calculate silent update
    if (move.taken() == 0 && piece_moved != 1 && piece_moved != 7) {
        silent += 1;
    }
    else {
        silent = 0;
    }
    //handle upgrade moves
    if (move.upgrade()) {
        removePiece(move.from());
        addPiece(move.to(), move.upgrade() + color_mod);
    }
    else {
        u8 old_ep = ep;
        ep = 8;
        switch (piece_moved)
        {
        case 1: //white pawn
            if (move.to() - move.from() != 8) {     //not normal moved pawn
                if (move.to() - move.from() == 16) { //Double moved pawn
                    ep = move.to() % 8;
                    removePiece(move.from());
                    addPiece(move.to(), 1);
                }
                else if (move.to() % 8 != old_ep || move.to() / 8 != 5) {
                    removePiece(move.from());
                    addPiece(move.to(), 1);
                }
                else {  //EP
                    removePiece(move.from());
                    addPiece(move.to(), 1);
                    removePiece(move.to() - 8);
                }
            }
            else {
                removePiece(move.from());
                addPiece(move.to(), 1);
            }
            break;
        case 7: //black pawn
            if (move.from() - move.to() != 8) {     //not normal moved pawn
                if (move.from() - move.to() == 16) { //Double moved pawn
                    ep = move.to() % 8;
                    removePiece(move.from());
                    addPiece(move.to(), 7);
                }
                else if (move.to() % 8 != old_ep || move.to() / 8 != 2) {
                    removePiece(move.from());
                    addPiece(move.to(), 7);
                }
                else {  //EP
                    removePiece(move.from());
                    addPiece(move.to(), 7);
                    removePiece(move.to() + 8);
                }
            }
            else {
                removePiece(move.from());
                addPiece(move.to(), 7);
            }
            break;
        case 6: //white king
            if (move.from() == 4 && move.to() == 2) { //Queen side rockad
                removePiece(4);
                removePiece(0);
                addPiece(3, 4);
                addPiece(2, 6);
            }
            else if (move.from() == 4 && move.to() == 6) {  //king side rockad
                removePiece(4);
                removePiece(7);
                addPiece(5, 4);
                addPiece(6, 6);
            }
            else {
                removePiece(move.from());
                addPiece(move.to(), 6);
            }
            break;
        case 12:
            if (move.from() == 60 && move.to() == 58) { //Queen side rockad
                removePiece(60);
                removePiece(56);
                addPiece(59, 10);
                addPiece(58, 12);
            }
            else if (move.from() == 60 && move.to() == 62) { //king side rockad
                removePiece(63);
                removePiece(60);
                addPiece(61, 10);
                addPiece(62, 12);
            }
            else {
                removePiece(move.from());
                addPiece(move.to(), 12);
            }
            break;
        default:
            removePiece(move.from());
            addPiece(move.to(), piece_moved);
            break;
        }
    }
    color = !color;
}

void BitBoard::unmake(Move move) {
    //uppdaterar silent
    silent = silent_mem[silent_mem.size() - 1];
    silent_mem.pop_back();

    //update ep
    ep = move.ep();
    u8 color_mod = (!color) * 6;
    u8 taken_piece;
    if (move.taken()) {
        taken_piece = move.taken() + color_mod;
    }
    else {
        taken_piece = 0;
    }
    if (move.upgrade()) {
        addPiece(move.to(), taken_piece);
        addPiece(move.from(), 1 + color_mod);
    }
    else {
        u8 piece_moved = mailboard_var.pieces[move.to()];
        switch (piece_moved)
        {
        case 1: //white pawn
            if (move.to() - move.from() != 8) {     //not normal moved pawn
                if (move.to() - move.from() == 16) { //Double moved pawn
                    removePiece(move.to());
                    addPiece(move.from(), 1);
                }
                else if (move.to() % 8 != ep || move.to() / 8 != 5) {
                    addPiece(move.to(), taken_piece);
                    addPiece(move.from(), 1);
                }
                else {  //EP
                    removePiece(move.to());
                    addPiece(move.from(), 1);
                    addPiece(move.to() - 8, 7);
                }
            }
            else {
                removePiece(move.to());
                addPiece(move.from(), 1);
            }
            break;
        case 7: //black pawn
            if (move.from() - move.to() != 8) {     //not normal moved pawn
                if (move.from() - move.to() == 16) { //Double moved pawn
                    removePiece(move.to());
                    addPiece(move.from(), 7);
                }
                else if (move.to() % 8 != ep || move.to() / 8 != 2) {
                    addPiece(move.to(), taken_piece);
                    addPiece(move.from(), 7);
                }
                else {  //EP
                    removePiece(move.to());
                    addPiece(move.from(), 7);
                    addPiece(move.to() + 8, 1);
                }
            }
            else {
                removePiece(move.to());
                addPiece(move.from(), 7);
            }
            break;
        case 6: //white king
            if (move.from() == 4 && move.to() == 2) { //Queen side rockad
                removePiece(3);
                removePiece(2);
                addPiece(0, 4);
                addPiece(4, 6);
            }
            else if (move.from() == 4 && move.to() == 6) {  //king side rockad
                removePiece(5);
                removePiece(6);
                addPiece(4, 6);
                addPiece(7, 4);
            }
            else {
                addPiece(move.to(), taken_piece);
                addPiece(move.from(), 6);
            }
            break;
        case 12:
            if (move.from() == 60 && move.to() == 58) { //Queen side rockad
                removePiece(59);
                removePiece(58);
                addPiece(56, 10);
                addPiece(60, 12);
            }
            else if (move.from() == 60 && move.to() == 62) { //king side rockad
                removePiece(61);
                removePiece(62);
                addPiece(63, 10);
                addPiece(60, 12);
            }
            else {
                addPiece(move.to(), taken_piece);
                addPiece(move.from(), 12);
            }
            break;
        default:
            addPiece(move.to(), taken_piece);
            addPiece(move.from(), piece_moved);
            break;
        }
    }
    color = !color;
}
/**
 * @brief recursively calculates perft for given board and depth
 * 
 * @param board 
 * @param depth 
 * @param move_start 
 * @return u64 
 */

u64 _perftHelp(BitBoard &board, u64 depth, Move *move_start) {
    if (depth == 0) {
        return 1;
    }
    Move *end = board.moveGen(move_start);
    u64 res = 0;
    while (move_start < end) {
        board.make(*move_start);
        res += _perftHelp(board, depth - 1, end);
        board.unmake(*move_start);
    }
    return res;
}


/**
 * @brief Peforms perft for given position without leaf node optimizations
 * 
 * @param board 
 * @param depth
 * @return u64 
 */
u64 perft(BitBoard &board, u64 depth) {
    Move moves[100000];
    return _perftHelp(board, depth, moves);
    delete[] moves;
}

/**
 * @brief recursively calculates perft for given board and depth
 * 
 * @param board 
 * @param depth 
 * @param move_start 
 * @return u64 
 */
u64 _perftLeafHelp(BitBoard &board, u64 depth, Move *move_start) {
    if (depth == 0) {
        return 1;
    }
    Move *end = board.moveGen(move_start);
    if (depth == 1) {
        return (end - move_start);
    }
    u64 res = 0;
    while (move_start < end) {
        board.make(*move_start);
        res += _perftLeafHelp(board, depth - 1, end);
        board.unmake(*move_start);
    }
    return res;
}

/**
 * @brief Peforms perft for given position with leaf node optimizations
 * 
 * @param board 
 * @param depth
 * @return u64 
 */
u64 perftLeaf(BitBoard &board, u64 depth) {
    Move moves[100000];
    return _perftLeafHelp(board, depth, moves);
    delete[] moves;  
};
