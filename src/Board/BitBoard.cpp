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

void initiate();

void BitBoard::add_piece(size_t index, u8 piece) {
    u8 removed = mailboard_var.pieces[index];
    mailboard_var.pieces[index] = piece; //Mail board updated

    bitboard_var.pieces[removed] &= ~(ONE << index);
    bitboard_var.pieces[piece] |= ONE << index;

    //Modify zoobrist hash

}

void BitBoard::remove_piece(size_t index) {
    add_piece(index, 0);
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
        remove_piece(move.from());
        add_piece(move.to(), move.upgrade() + color_mod);
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
                    remove_piece(move.from());
                    add_piece(move.to(), 1);
                }
                else if (move.to() % 8 != old_ep || move.to() / 8 != 5) {
                    remove_piece(move.from());
                    add_piece(move.to(), 1);
                }
                else {  //EP
                    remove_piece(move.from());
                    add_piece(move.to(), 1);
                    remove_piece(move.to() - 8);
                }
            }
            else {
                remove_piece(move.from());
                add_piece(move.to(), 1);
            }
            break;
        case 7: //black pawn
            if (move.from() - move.to() != 8) {     //not normal moved pawn
                if (move.from() - move.to() == 16) { //Double moved pawn
                    ep = move.to() % 8;
                    remove_piece(move.from());
                    add_piece(move.to(), 7);
                }
                else if (move.to() % 8 != old_ep || move.to() / 8 != 2) {
                    remove_piece(move.from());
                    add_piece(move.to(), 7);
                }
                else {  //EP
                    remove_piece(move.from());
                    add_piece(move.to(), 7);
                    remove_piece(move.to() + 8);
                }
            }
            else {
                remove_piece(move.from());
                add_piece(move.to(), 7);
            }
            break;
        case 6: //white king
            if (move.from() == 4 && move.to() == 2) { //Queen side rockad
                remove_piece(4);
                remove_piece(0);
                add_piece(3, 4);
                add_piece(2, 6);
            }
            else if (move.from() == 4 && move.to() == 6) {  //king side rockad
                remove_piece(4);
                remove_piece(7);
                add_piece(5, 4);
                add_piece(6, 6);
            }
            else {
                remove_piece(move.from());
                add_piece(move.to(), 6);
            }
            break;
        case 12:
            if (move.from() == 60 && move.to() == 58) { //Queen side rockad
                remove_piece(60);
                remove_piece(56);
                add_piece(59, 10);
                add_piece(58, 12);
            }
            else if (move.from() == 60 && move.to() == 62) { //king side rockad
                remove_piece(63);
                remove_piece(60);
                add_piece(61, 10);
                add_piece(62, 12);
            }
            else {
                remove_piece(move.from());
                add_piece(move.to(), 12);
            }
            break;
        default:
            remove_piece(move.from());
            add_piece(move.to(), piece_moved);
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
        add_piece(move.to(), taken_piece);
        add_piece(move.from(), 1 + color_mod);
    }
    else {
        u8 piece_moved = mailboard_var.pieces[move.to()];
        switch (piece_moved)
        {
        case 1: //white pawn
            if (move.to() - move.from() != 8) {     //not normal moved pawn
                if (move.to() - move.from() == 16) { //Double moved pawn
                    remove_piece(move.to());
                    add_piece(move.from(), 1);
                }
                else if (move.to() % 8 != ep || move.to() / 8 != 5) {
                    add_piece(move.to(), taken_piece);
                    add_piece(move.from(), 1);
                }
                else {  //EP
                    remove_piece(move.to());
                    add_piece(move.from(), 1);
                    add_piece(move.to() - 8, 7);
                }
            }
            else {
                remove_piece(move.to());
                add_piece(move.from(), 1);
            }
            break;
        case 7: //black pawn
            if (move.from() - move.to() != 8) {     //not normal moved pawn
                if (move.from() - move.to() == 16) { //Double moved pawn
                    remove_piece(move.to());
                    add_piece(move.from(), 7);
                }
                else if (move.to() % 8 != ep || move.to() / 8 != 2) {
                    add_piece(move.to(), taken_piece);
                    add_piece(move.from(), 7);
                }
                else {  //EP
                    remove_piece(move.to());
                    add_piece(move.from(), 7);
                    add_piece(move.to() + 8, 1);
                }
            }
            else {
                remove_piece(move.to());
                add_piece(move.from(), 7);
            }
            break;
        case 6: //white king
            if (move.from() == 4 && move.to() == 2) { //Queen side rockad
                remove_piece(3);
                remove_piece(2);
                add_piece(0, 4);
                add_piece(4, 6);
            }
            else if (move.from() == 4 && move.to() == 6) {  //king side rockad
                remove_piece(5);
                remove_piece(6);
                add_piece(4, 6);
                add_piece(7, 4);
            }
            else {
                add_piece(move.to(), taken_piece);
                add_piece(move.from(), 6);
            }
            break;
        case 12:
            if (move.from() == 60 && move.to() == 58) { //Queen side rockad
                remove_piece(59);
                remove_piece(58);
                add_piece(56, 10);
                add_piece(60, 12);
            }
            else if (move.from() == 60 && move.to() == 62) { //king side rockad
                remove_piece(61);
                remove_piece(62);
                add_piece(63, 10);
                add_piece(60, 12);
            }
            else {
                add_piece(move.to(), taken_piece);
                add_piece(move.from(), 12);
            }
            break;
        default:
            add_piece(move.to(), taken_piece);
            add_piece(move.from(), piece_moved);
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
u64 _perft_help(BitBoard &board, u64 depth, Move *move_start) {
    if (depth == 0) {
        return 1;
    }
    Move *end = board.move_gen(move_start);
    u64 res = 0;
    while (move_start < end) {
        board.make(*move_start);
        res += _perft_help(board, depth - 1, end);
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
    return _perft_help(board, depth, moves);
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
u64 _perft_leaf_help(BitBoard &board, u64 depth, Move *move_start) {
    if (depth == 0) {
        return 1;
    }
    Move *end = board.move_gen(move_start);
    if (depth == 1) {
        return (end - move_start);
    }
    u64 res = 0;
    while (move_start < end) {
        board.make(*move_start);
        res += _perft_leaf_help(board, depth - 1, end);
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
u64 perft_leaf(BitBoard &board, u64 depth) {
    Move moves[100000];
    return _perft_leaf_help(board, depth, moves);
    delete[] moves;  
};
