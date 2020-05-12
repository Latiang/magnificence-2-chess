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
#pragma once
#include "BitBoard.h"
#include "../Interface/BoardConversions.h"

bool has_initiated = false;
std::mt19937_64 rng(2348723472648796678);
u64 zoobrist_keys[64 * 12 + 8 + 4];
u64 king_masks[64];
u64 knight_masks[64];
u64 bishop_masks[64];
u64 rook_masks[64];
std::vector<u64> bishop_magic[64];
std::vector<u64> rook_magic[64];
u64 columns[8];
u64 rows[8];

/**
 * @brief Helper function for findAllVariations
 * 
 * @param indexes 
 * @param out 
 * @param mask 
 * @param read_index 
 */
void findAllVariationsHelp(const std::vector<u8> &indexes, std::vector<u64> &out, u64 mask, size_t read_index) {
    if (read_index == indexes.size()) {
        out.push_back(mask);
    }
    else {
        findAllVariationsHelp(indexes, out, mask, read_index + 1);
        mask ^= ONE << (indexes[read_index]);
        findAllVariationsHelp(indexes, out, mask, read_index + 1);
    }
}

/**
 * @brief Finds all possible permutations of u64 where only the specified indexes may be set
 * 
 * @param indexes 
 * @param out 
 */
void findAllVariations(const std::vector<u8> &indexes, std::vector<u64> &out) {
    findAllVariationsHelp(indexes, out, 0, 0);
}


void init() {
    if (has_initiated) {
        return;
    }
    has_initiated = true;
    //initialise zoobrist keys
    for (size_t i = 0; i < 780; i++)
    {
        zoobrist_keys[i] = rng();
    }
    for (size_t i = 0; i < 8; i++)
    {
        rows[i] = ((u64)0b11111111) << (8 * i);
        columns[i] = 0;
        for (size_t j = 0; j < 8; j++)
        {
            columns[i] |= ONE << (i + 8 * j);
        }
        int test = 10;
    }
    for (size_t i = 0; i < 64; i++)
    {
        //king
        int x = i % 8;
        int y = i / 8;
        king_masks[i] = 0;
        for (int lx = x - 1; lx <= x + 1; lx++)
        {
            for (int ly = y - 1; ly <= y + 1; ly++)
            {
                if (ly >= 0 && ly <= 7 && lx >= 0 && lx <= 7 && (ly != y || lx != x)) {
                    king_masks[i] |= ONE << (lx + ly * 8);
                }
            }
        }

        //knight
        for (size_t j = 0; j < 2; j++)
        {
            int lx, ly;
            int mod1 = ((int)(4 * j))  - 2;
            for (size_t k = 0; k < 2; k++)
            {
                int mod2 = ((int)(2 * k))  - 1;
                lx = x + mod1;
                ly = y + mod2;
                if (ly >= 0 && ly <= 7 && lx >= 0 && lx <= 7 && (ly != y || lx != x)) {
                    knight_masks[i] |= ONE << (lx + ly * 8);
                }
                lx = x + mod2;
                ly = y + mod1;
                if (ly >= 0 && ly <= 7 && lx >= 0 && lx <= 7 && (ly != y || lx != x)) {
                    knight_masks[i] |= ONE << (lx + ly * 8);
                }
            }
        }
        
        u64 position = ONE << i;

        //bishop

        //up left
        u64 mask = 0;
        u64 stop_mask = ~(columns[0] | rows[7]);
        u64 mem = position & stop_mask;
        while (mem) {
            mem = (mem << 7) & stop_mask;
            mask |= mem;
        }
        //up right
        stop_mask = ~(columns[7] | rows[7]);
        mem = position & stop_mask;
        while (mem) {
            mem = (mem << 9) & stop_mask;
            mask |= mem;
        }
        //down left
        stop_mask = ~(columns[0] | rows[0]);
        mem = position & stop_mask;
        while (mem) {
            mem = (mem >> 9) & stop_mask;
            mask |= mem;
        }
        //down right
        stop_mask = ~(columns[7] | rows[0]);
        mem = position & stop_mask;
        while (mem) {
            mem = (mem >> 7) & stop_mask;
            mask |= mem;
        }
        bishop_masks[i] = mask;
        std::vector<u8>indexes;
        while (mask)
        {
            u8 index = bitScanForward(mask);
            mask &= mask - 1;
            indexes.push_back(index);
        }
        std::vector<u64> variations;
        findAllVariations(indexes, variations);
        #if defined(DEBUG)
        assert(variations.size() == (ONE << populationCount(bishop_masks[i])));
        assert((position & bishop_masks[i]) == 0);
        #endif
        bishop_magic[i] = std::vector<u64>(variations.size());
        for (u64 variation: variations) {
            u64 magic_val = pext(variation, bishop_masks[i]);
            u64 options = 0;
            //up left
            stop_mask = ~(columns[0] | rows[7]);
            stop_mask &= ~variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem << 7;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //up right
            stop_mask = ~(columns[7] | rows[7]);
            stop_mask &= ~variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem << 9;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //down left
            stop_mask = ~(columns[0] | rows[0]);
            stop_mask &= ~variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem >> 9;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //down right
            stop_mask = ~(columns[7] | rows[0]);
            stop_mask &= ~variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem >> 7;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            bishop_magic[i][magic_val] = options;
        }
        

        //rook
        mask = 0;
        //up
        stop_mask = ~rows[7];
        mem  = position & stop_mask;
        while (mem) {
            mem = (mem << 8) & stop_mask;
            mask |= mem;
        }
        //down
        stop_mask = ~rows[0];
        mem  = position & stop_mask;
        while (mem) {
            mem = (mem >> 8) & stop_mask;
            mask |= mem;
        }
        //left
        stop_mask = ~columns[0];
        mem  = position & stop_mask;
        while (mem) {
            mem = (mem >> 1) & stop_mask;
            mask |= mem;
        }
        //right
        stop_mask = ~columns[7];
        mem  = position & stop_mask;
        while (mem) {
            mem = (mem << 1) & stop_mask;
            mask |= mem;
        }
        rook_masks[i] = mask;
        variations = std::vector<u64>();
        indexes = std::vector<u8>();
        while (mask) {
            indexes.push_back(bitScanForward(mask));
            mask &= mask - 1;
        }
        findAllVariations(indexes, variations);
        #if defined(DEBUG)
        assert(variations.size() == (ONE << populationCount(rook_masks[i])));
        assert((position & rook_masks[i]) == 0);
        #endif
        rook_magic[i] = std::vector<u64>(variations.size());
        for (u64 variation : variations) {
            u64 magic_val = pext(variation, rook_masks[i]);
            u64 options = 0;
            //up
            stop_mask = ~(rows[7]);
            stop_mask &= ~variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem << 8;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //right
            stop_mask = ~(columns[7]);
            stop_mask &= ~variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem << 1;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //down
            stop_mask = ~(rows[0]);
            stop_mask &= ~variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem >> 8;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //left
            stop_mask = ~(columns[0]);
            stop_mask &= ~variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem >> 1;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            rook_magic[i][magic_val] = options;
        }
    }
    
}


/**
 * @brief Adds the given piece at index to the board
 * 
 * @param index 
 * @param piece 
 */
void BitBoard::addPiece(size_t index, u8 piece) {
    u8 removed = mailboard_var.pieces[index];
    mailboard_var.pieces[index] = piece; //Mail board updated

    bitboard_var.pieces[removed] &= ~(ONE << index);
    bitboard_var.pieces[piece] |= ONE << index;

    //Modify zoobrist hash

}

/**
 * @brief Removes the piece at the specified index
 * 
 * @param index 
 */
void BitBoard::removePiece(size_t index) {
    addPiece(index, 0);
}


/**
 * @brief Creates a new BitBoard in the startposition
 * 
 */
BitBoard::BitBoard() {
    init();
    (*this) =  BitBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    int x = 1;
}

/**
 * @brief Copies the given bitboard
 * 
 * @param original 
 */
BitBoard::BitBoard(const BitBoard &original) {
    init();
    (*this) = original;
}

/**
 * @brief Creates a board from a 
 * 
 * _string
 * 
 * @param fen_string 
 */
BitBoard::BitBoard(const std::string &fen_string) {
    init();
    size_t index = 56;
    silent = 0;
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
    if (i >= fen_string.size())
    {
        color = true;
        ep = 8;
        castling = 15;
        initZoobrist();
        return;
    }
    if (fen_string[i] == 'w') {
        color = true;
    }
    else {
        color = false;
    }
    castling = 0;
    i += 2;
    if (i >= fen_string.size())
    {
        ep = 8;
        castling = 15;
        initZoobrist();
        return;
    }
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
    if (i >= fen_string.size()) {
        initZoobrist();
        return;
    }
    if (fen_string[i] != '-') {
        ep = fen_string[i] - 97;
        i += 3;
    }
    else {
        ep = 8;
        i += 2;
    }
    if (i >= fen_string.size()) {
        initZoobrist();
        return;
    }
    while (i < fen_string.size() && fen_string[i] != ' ') {
        silent *= 10;
        silent += fen_string[i] - 48;
        i += 1;
    }
    initZoobrist();
}

void BitBoard::initZoobrist()
{
    zoobrist = 0;
    for (size_t i = 0; i < 13; i++)
    {
        bitboard_var[i] = 0;
    }
    bitboard_var[0] = FULL;
    for (size_t i = 0; i < 64; i++)
    {
        addPiece(i, mailboard_var[i]);  //sets zoobrist hash and 
    }
    int some = 1;
}

/**
 * @brief Makes the given move
 * 
 * @param move 
 */
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
    u8 color_mod = (!color) * 6;

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

/**
 * @brief Unmakes the given move
 * 
 * @param move 
 */
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
        addPiece(move.from(), 1 + 6 * color);
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
 * @brief Returns the reachable positions for a bishop at position givven occupancy mask
 * 
 * @param position 
 * @param occupancy_mask 
 * @return u64 
 */
inline u64 bishopMovesReachable(u8 position, u64 occupancy_mask) {
    u64 mask = bishop_masks[position];
    size_t index = pext(occupancy_mask, bishop_masks[position]);
    return bishop_magic[position][index];
}


/**
 * @brief Returns the reachable positions for a rook at position givven occupancy mask
 * 
 * @param position
 * @param occupancy_mask 
 * @return u64 
 */
inline u64 rookMovesReachable(u8 position, u64 occupancy_mask) {
    return rook_magic[position][pext(occupancy_mask, rook_masks[position])];
}


/**
 * @brief Finds the locked pieces
 * 
 * @param king_mask, the king mask of current player
 * @param occupancy_mask 
 * @param bishop_like_pieces, queen and bishop of the other player
 * @param rook_like_pieces, rook and queen of the other player
 * @return u64 
 */
u64 findLockedPieces(u8 king_index, u64 occupancy_mask, u64 bishop_like_pieces, u64 rook_like_pieces) {
    u64 blocked = 0;
    u64 reachable_pieces = bishopMovesReachable(king_index, occupancy_mask) & occupancy_mask;
    //we have now established the pieces potentially blocked by a bishop_like_piece
    //now we must find wheter there is such a piece
    while (bishop_like_pieces) {
        u8 bishop_index = bitScanForward(bishop_like_pieces);
        u64 takeable_opponent = bishopMovesReachable(bishop_index, occupancy_mask);
        u64 potential_block = takeable_opponent & reachable_pieces;
        if (potential_block) {
            u8 x_difference = std::max(bishop_index % 8, king_index % 8) - std::min(bishop_index % 8, king_index % 8);
            u8 y_difference = std::max(bishop_index / 8, king_index / 8) - std::min(bishop_index / 8, king_index / 8);
            if (x_difference == y_difference) {
                blocked |= potential_block;
            }
        }
        bishop_like_pieces &= bishop_like_pieces - 1;
    }
    reachable_pieces = rookMovesReachable(king_index, occupancy_mask) & occupancy_mask;
    while (rook_like_pieces) {
        u8 rook_index = bitScanForward(rook_like_pieces);
        u64 takeable_opponent = rookMovesReachable(rook_index, occupancy_mask);
        u64 potential_block = takeable_opponent & reachable_pieces;
        if (potential_block && (rook_index % 8 == king_index % 8 || rook_index / 8 == king_index / 8)) {
            blocked |= potential_block;
        }
        rook_like_pieces &= rook_like_pieces - 1;
    }
    return blocked;
}

inline u64 rookLikeMoves(u8 rook_index, u8 king_index, u64 occupancy_mask, u64 valid_targets, u64 locked) {
    u64 reachable = rookMovesReachable(rook_index, occupancy_mask) & valid_targets; //can move anywhere except own piece
    if ((ONE << rook_index) & locked) {
        if (rook_index % 8 == king_index % 8) {
            return reachable & columns[rook_index % 8];
        }
        else {
            return reachable & rows[rook_index / 8];
        }
    }
    else {
        return reachable;
    }
}

inline u64 bishopLikeMoves(u8 bishop_index, u8 king_index, u64 occupancy_mask, u64 valid_targets, u64 locked) {
    u64 reachable = bishopMovesReachable(bishop_index, occupancy_mask) & valid_targets;
    if ((ONE << bishop_index) & locked) {
        occupancy_mask &= ~(ONE << bishop_index);
        u64 king_reachable = bishopMovesReachable(king_index, occupancy_mask);
        return king_reachable & reachable;
    }
    else {
        return reachable;
    }
}



 /**
 * @brief Generates legal moves for white
 * 
 * @param move_start_buffer moves will be inserted with start here and new moves will be written to following adresses
 * @return Move* returns adress after the last move inserted
 */
Move *BitBoard::moveGenWhite(Move *move_buffer) {
    //We need to identify locked pieces
    //The only pieces which can lock are 
    //the sliding pieces
    Move *move_buffer_start = move_buffer;
    u64 occupancy_mask_white = bitboard_var[1] | bitboard_var[2] | bitboard_var[3] | bitboard_var[4] | bitboard_var[5] | bitboard_var[6];
    u64 occupancy_mask_black = bitboard_var[7] | bitboard_var[8] | bitboard_var[9] | bitboard_var[10] | bitboard_var[11] | bitboard_var[12];
    u64 occupancy_mask = occupancy_mask_white | occupancy_mask_black;
    u64 opponent_bishop_like = bitboard_var[9] | bitboard_var[11];
    u64 opponent_rook_like = bitboard_var[10] | bitboard_var[11];
    u8 king_index = bitScanForward(bitboard_var[6]);
    u64 locked = findLockedPieces(king_index, occupancy_mask, opponent_bishop_like, opponent_rook_like);
    Move base_move;
    base_move.setEP(ep);
    base_move.setCastling(castling);
    u8 checks = 0;
    u64 mem = opponent_rook_like;
    u64 threatened = king_masks[bitScanForward(bitboard_var[12])];  //opponent king threat
    u64 check_mask = FULL;
    occupancy_mask &= ~(bitboard_var[6]); //remove king in order to properly find threatened squares
    while (mem) {   //opponents rooks
        u8 rook_index = bitScanForward(mem);
        u64 newMoves = rookMovesReachable(rook_index, occupancy_mask);
        threatened |= newMoves;
        if (newMoves & bitboard_var[6]) {
            check_mask = (rookMovesReachable(king_index, occupancy_mask) & rookMovesReachable(rook_index, occupancy_mask | bitboard_var[6])) | (ONE << rook_index);
            checks += 1;
        }
        mem &= mem - 1;
    }
    mem = opponent_bishop_like;
    while (mem) {   //opponent bishop
        u8 bishop_index = bitScanForward(mem);
        u64 newMoves = bishopMovesReachable(bishop_index, occupancy_mask);
        threatened |= newMoves;
        if (newMoves & bitboard_var[6]) {
            check_mask = (bishopMovesReachable(king_index, occupancy_mask) & bishopMovesReachable(bishop_index, occupancy_mask | bitboard_var[6])) | (ONE << bishop_index);
            checks += 1;
        }
        mem &= mem - 1;
    }
    mem = bitboard_var[8];
    while (mem) {   //opponent knight
        u64 newMoves = knight_masks[bitScanForward(mem)];
        threatened |= newMoves;
        if (newMoves & bitboard_var[6]) {
            checks += 1;
            check_mask = ONE << bitScanForward(mem);
        }
        mem &= mem - 1;
    }
    {   //opponent pawn
        u64 newMoves = ((bitboard_var[7] >> 9) & ~(columns[7])) | ((bitboard_var[7] >> 7) & ~(columns[0]));
        threatened |= newMoves;
        if (newMoves & bitboard_var[6]) {
            check_mask = (((bitboard_var[6] << 9) & ~(columns[0])) | ((bitboard_var[6] << 7) & ~(columns[7]))) & bitboard_var[7];
            checks += 1;
        }
    }


    occupancy_mask |= bitboard_var[6];      //add king
    //king moves
    u64 valid_moves = king_masks[king_index] & (~threatened) & (~occupancy_mask_white); //may not move to a threatened piece or own piece
    base_move.setFrom(king_index);
    while (valid_moves) {
        u8 to = bitScanForward(valid_moves);
        base_move.setTo(to);
        valid_moves &= valid_moves - 1;
        u8 taken = mailboard_var[to];
        if (taken) {
            base_move.setTaken(taken - 6);
        }
        else {
            base_move.setTaken(taken);
        }
        *move_buffer = base_move;
        move_buffer++;
    }
    if (!checks) {
        //castling
        if (castling)
        {
            
        }
    }
    else if (checks >= 2) {
        //handle doubble check, only king may move
        if (move_buffer == move_buffer_start) {
            *move_buffer = Move();
        }
        return move_buffer;
    }
    u64 valid_targets = check_mask & (~occupancy_mask_white);
    //find rook_moves
    mem = bitboard_var[5] | bitboard_var[4];
    while (mem) {
        base_move.setFrom(bitScanForward(mem));
        u64 moves = rookLikeMoves(base_move.from(), king_index, occupancy_mask, valid_targets, locked);
        while (moves) {
            base_move.setTo(bitScanForward(moves));
            moves &= moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            if (taken) {
                base_move.setTaken(taken - 6);
            }
            else {
                base_move.setTaken(taken);
            }
            *move_buffer = base_move;
            move_buffer++;
        }
        mem &= mem - 1;
    }
    //find bishop moves
    mem = bitboard_var[3] | bitboard_var[5];
    while (mem) {
        base_move.setFrom(bitScanForward(mem));
        u64 moves = bishopLikeMoves(base_move.from(), king_index, occupancy_mask, valid_targets, locked);
        while (moves) {
            base_move.setTo(bitScanForward(moves));
            moves &= moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            if (taken) {
                base_move.setTaken(taken - 6);
            }
            else {
                base_move.setTaken(taken);
            }
            *move_buffer = base_move;
            move_buffer++;
        }
        mem &= mem - 1;
    }
    //find knight moves
    mem = bitboard_var[2];
    while (mem) {
        base_move.setFrom(bitScanForward(mem));
        if ((ONE << base_move.from()) & locked) {
            mem &= mem - 1;
            continue;
        }
        u64 moves = knight_masks[base_move.from()] & valid_targets;
        while (moves) {
            base_move.setTo(bitScanForward(moves));
            moves &= moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            if (taken) {
                base_move.setTaken(taken - 6);
            }
            else {
                base_move.setTaken(taken);
            }
            *move_buffer = base_move;
            move_buffer++;
        }
        mem &= mem - 1;
    }
    //find pawn moves
    {
        u64 locked_pawns = bitboard_var[1] & locked;
        u64 pawns = bitboard_var[1] & (~locked);
        //agressive left
        u64 legal_moves = ((pawns << 7) & (~columns[7])) & valid_targets & occupancy_mask_black;
        while (legal_moves) {

            base_move.setTo(bitScanForward(legal_moves));
            base_move.setFrom(base_move.to() - 7);
            legal_moves &= legal_moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            if (taken) {
                base_move.setTaken(taken - 6);
            }
            else {
                base_move.setTaken(taken);
            }
            if (base_move.to() / 8 == 7) {
                for (size_t i = 2; i < 6; i++)
                {
                    base_move.setUpgrade(i);
                    *move_buffer = base_move;
                    move_buffer++;
                }
                base_move.setUpgrade(0);
            }
            else {
                *move_buffer = base_move;
                move_buffer++;
            }
        }
        legal_moves = ((pawns << 9) & (~columns[0])) & valid_targets & occupancy_mask_black;
        while (legal_moves) {
            base_move.setTo(bitScanForward(legal_moves));
            base_move.setFrom(base_move.to() - 9);
            legal_moves &= legal_moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            if (taken) {
                base_move.setTaken(taken - 6);
            }
            else {
                base_move.setTaken(taken);
            }
            if (base_move.to() / 8 == 7) {
                for (size_t i = 2; i < 6; i++)
                {
                    base_move.setUpgrade(i);
                    *move_buffer = base_move;
                    move_buffer++;
                }
                base_move.setUpgrade(0);
            }
            else {
                *move_buffer = base_move;
                move_buffer++;
            }
        }
        legal_moves = ((pawns << 8)) & (~occupancy_mask);
        mem = legal_moves;
        legal_moves &= valid_targets;
        while (legal_moves) {
            base_move.setTo(bitScanForward(legal_moves));
            base_move.setFrom(base_move.to() - 8);
            u8 taken = 0;
            base_move.setTaken(taken);
            legal_moves &= legal_moves - 1;
            if (base_move.to() / 8 == 7) {
                for (size_t i = 2; i < 6; i++)
                {
                    base_move.setUpgrade(i);
                    *move_buffer = base_move;
                    move_buffer++;
                }
                base_move.setUpgrade(0);
            }
            else {
                *move_buffer = base_move;
                move_buffer++;
            }
        }
        legal_moves = (mem << 8) & rows[3] & valid_targets & (~occupancy_mask);
        while (legal_moves) {
            base_move.setTo(bitScanForward(legal_moves));
            base_move.setFrom(base_move.to() - 16);
            u8 taken = 0;
            base_move.setTaken(taken);
            *move_buffer = base_move;
            move_buffer++;
            legal_moves &= legal_moves - 1;
        }
        if (ep != 8) {
            u64 all_pawns = bitboard_var[1];
            u64 ep_mask = ONE << (ep + 5 * 8);
            u64 takers = all_pawns & (((ep_mask >> 7) & (~columns[0])) | ((ep_mask >> 9) & (~columns[7])));
            while (takers) {
                u8 from = bitScanForward(takers);
                takers &= takers - 1;
                //we want to test wheter we can perform the operation and not be in check
                u64 occupancy_mask_sim = (occupancy_mask & (~(ONE << from)) & (~(ep_mask >> 8))) | ep_mask;
                u64 king_bishop = bishopMovesReachable(king_index, occupancy_mask_sim);
                u64 king_rook = rookMovesReachable(king_index, occupancy_mask_sim);
                if ((king_bishop & opponent_bishop_like) || (king_rook & opponent_rook_like)) {
                    continue;
                }
                base_move.setFrom(from);
                base_move.setTo(ep + 5 * 8);
                u8 taken = mailboard_var[base_move.to()];
                if (taken) {
                    base_move.setTaken(taken - 6);
                }
                else {
                    base_move.setTaken(taken);
                }
                *move_buffer = base_move;
                move_buffer++;
            }
        }
        while (locked_pawns) {
            base_move.setFrom(bitScanForward(locked_pawns));
            locked_pawns &= locked_pawns - 1;
            if (base_move.from() / 8 == king_index / 8) {
                continue;
            }
            else if (base_move.from() % 8 == king_index % 8) {
                //there can be no promotion in this case
                legal_moves = (ONE << (base_move.from() + 8)) & (~occupancy_mask);
                mem = legal_moves;
                legal_moves &= valid_targets;
                while (legal_moves) {
                    base_move.setTo(bitScanForward(legal_moves));
                    base_move.setFrom(base_move.to() - 8);
                    u8 taken = 0;
                    base_move.setTaken(taken);
                    *move_buffer = base_move;
                    move_buffer++;
                    legal_moves &= legal_moves - 1;
                }
                legal_moves = (mem << 8) & valid_targets & rows[3] & (~occupancy_mask);
                while (legal_moves) {
                    base_move.setTo(bitScanForward(legal_moves));
                    base_move.setFrom(base_move.to() - 16);
                    u8 taken = 0;
                    base_move.setTaken(taken);
                    *move_buffer = base_move;
                    move_buffer++;
                    legal_moves &= legal_moves - 1;
                }
                mem = legal_moves;

            }
            else if (base_move.from() % 8 < king_index % 8) {
                //pawn is to the left and may take to the left. This may cause an upgrade
                legal_moves = (ONE << (base_move.from() + 7)) & occupancy_mask_black & valid_targets;
                while (legal_moves) {
                    base_move.setTo(bitScanForward(legal_moves));
                    legal_moves &= legal_moves - 1;
                    u8 taken = mailboard_var[base_move.to()];
                    if (taken) {
                        base_move.setTaken(taken - 6);
                    }
                    else {
                        base_move.setTaken(0);
                    }
                    if (base_move.to() / 8 == 7) {
                        for (size_t i = 2; i < 6; i++)
                        {
                            base_move.setUpgrade(i);
                            *move_buffer = base_move;
                            move_buffer++;
                        }
                        base_move.setUpgrade(0);
                    }
                    else {
                        *move_buffer = base_move;
                        move_buffer++;
                    }
                }
            }
            else {
                //pawn is to the right and may take to the right. This may cause an upgrade
                legal_moves = (ONE << (base_move.from() + 9)) & occupancy_mask_black & valid_targets;
                while (legal_moves) {
                    base_move.setTo(bitScanForward(legal_moves));
                    legal_moves &= legal_moves - 1;
                    u8 taken = mailboard_var[base_move.to()];
                    if (taken) {
                        base_move.setTaken(taken - 6);
                    }
                    else {
                        base_move.setTaken(0);
                    }
                    if (base_move.to() / 8 == 7) {
                        for (size_t i = 2; i < 6; i++)
                        {
                            base_move.setUpgrade(i);
                            *move_buffer = base_move;
                            move_buffer++;
                        }
                        base_move.setUpgrade(0);
                    }
                    else {
                        *move_buffer = base_move;
                        move_buffer++;
                    }
                }
            }
        }
    }
    if (move_buffer == move_buffer_start) {
        if (checks) {
            *move_buffer = Move();
        }
        else {
            *move_buffer = Move();
            (*move_buffer).setFrom(1);
        }
    }
    return move_buffer;
}


/**
 * @brief Generates legal moves for black
 * 
 * @param move_buffer moves will be inserted with start here and new moves will be written to following adresses
 * @return Move* returns adress after the last move inserted
 */
Move * BitBoard::moveGenBlack(Move *move_buffer)  {
    //We need to identify locked pieces
    //The only pieces which can lock are 
    //the sliding pieces
    Move *move_buffer_start = move_buffer;
    u64 occupancy_mask_white = bitboard_var[1] | bitboard_var[2] | bitboard_var[3] | bitboard_var[4] | bitboard_var[5] | bitboard_var[6];
    u64 occupancy_mask_black = bitboard_var[7] | bitboard_var[8] | bitboard_var[9] | bitboard_var[10] | bitboard_var[11] | bitboard_var[12];
    u64 occupancy_mask = occupancy_mask_white | occupancy_mask_black;
    u64 opponent_bishop_like = bitboard_var[3] | bitboard_var[5];
    u64 opponent_rook_like = bitboard_var[4] | bitboard_var[5];
    u8 king_index = bitScanForward(bitboard_var[12]);
    u64 locked = findLockedPieces(king_index, occupancy_mask, opponent_bishop_like, opponent_rook_like);
    Move base_move;
    base_move.setEP(ep);
    base_move.setCastling(castling);
    u8 checks = 0;
    u64 mem = opponent_rook_like;
    u64 threatened = king_masks[bitScanForward(bitboard_var[6])];  //opponent king threat
    u64 check_mask = FULL;
    occupancy_mask &= ~(bitboard_var[12]); //remove king in order to properly find threatened squares
    while (mem) {   //opponents rooks
        u8 rook_index = bitScanForward(mem);
        u64 newMoves = rookMovesReachable(rook_index, occupancy_mask);
        threatened |= newMoves;
        if (newMoves & bitboard_var[12]) {
            check_mask = (rookMovesReachable(king_index, occupancy_mask) & rookMovesReachable(rook_index, occupancy_mask | bitboard_var[12])) | (ONE << rook_index);
            checks += 1;
        }
        mem &= mem - 1;
    }
    mem = opponent_bishop_like;
    while (mem) {   //opponent bishop
        u8 bishop_index = bitScanForward(mem);
        u64 newMoves = bishopMovesReachable(bishop_index, occupancy_mask);
        threatened |= newMoves;
        if (newMoves & bitboard_var[12]) {
            check_mask = (bishopMovesReachable(king_index, occupancy_mask) & bishopMovesReachable(bishop_index, occupancy_mask | bitboard_var[12])) | (ONE << bishop_index);
            checks += 1;
        }
        mem &= mem - 1;
    }
    mem = bitboard_var[2];
    while (mem) {   //opponent knight
        u64 newMoves = knight_masks[bitScanForward(mem)];
        threatened |= newMoves;
        if (newMoves & bitboard_var[12]) {
            checks += 1;
            check_mask = ONE << bitScanForward(mem);
        }
        mem &= mem - 1;
    }
    {   //opponent pawn
        u64 newMoves = ((bitboard_var[1] << 7) & ~(columns[7])) | ((bitboard_var[1] << 9) & ~(columns[0]));
        threatened |= newMoves;
        if (newMoves & bitboard_var[12]) {
            check_mask = (((bitboard_var[12] >> 7) & ~(columns[0])) | ((bitboard_var[12] >> 9) & ~(columns[7]))) & bitboard_var[1];
            checks += 1;
        }
    }


    occupancy_mask |= bitboard_var[12];      //add king
    //king moves
    u64 valid_moves = king_masks[king_index] & (~threatened) & (~occupancy_mask_black); //may not move to a threatened piece or own piece
    base_move.setFrom(king_index);
    while (valid_moves) {
        u8 to = bitScanForward(valid_moves);
        base_move.setTo(to);
        valid_moves &= valid_moves - 1;
        u8 taken = mailboard_var[to];
        base_move.setTaken(taken);
        *move_buffer = base_move;
        move_buffer++;
    }

    if (checks >= 2) {
        //handle doubble check, only king may move
        if (move_buffer == move_buffer_start) {
            *move_buffer = Move();
        }
        return move_buffer;
    }
    u64 valid_targets = check_mask & (~occupancy_mask_black);
    //find rook_moves
    mem = bitboard_var[11] | bitboard_var[10];
    while (mem) {
        base_move.setFrom(bitScanForward(mem));
        u64 moves = rookLikeMoves(base_move.from(), king_index, occupancy_mask, valid_targets, locked);
        while (moves) {
            base_move.setTo(bitScanForward(moves));
            moves &= moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            base_move.setTaken(taken);
            *move_buffer = base_move;
            move_buffer++;
        }
        mem &= mem - 1;
    }
    //find bishop moves
    mem = bitboard_var[9] | bitboard_var[11];
    while (mem) {
        base_move.setFrom(bitScanForward(mem));
        u64 moves = bishopLikeMoves(base_move.from(), king_index, occupancy_mask, valid_targets, locked);
        while (moves) {
            base_move.setTo(bitScanForward(moves));
            moves &= moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            base_move.setTaken(taken);
            *move_buffer = base_move;
            move_buffer++;
        }
        mem &= mem - 1;
    }
    //find knight moves
    mem = bitboard_var[8];
    while (mem) {
        base_move.setFrom(bitScanForward(mem));
        if ((ONE << base_move.from()) & locked) {
            mem &= mem - 1;
            continue;
        }
        u64 moves = knight_masks[base_move.from()] & valid_targets;
        while (moves) {
            base_move.setTo(bitScanForward(moves));
            moves &= moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            base_move.setTaken(taken);
            *move_buffer = base_move;
            move_buffer++;
        }
        mem &= mem - 1;
    }
    //find pawn moves
    {
        u64 locked_pawns = bitboard_var[7] & locked;
        u64 pawns = bitboard_var[7] & (~locked);
        //agressive left
        u64 legal_moves = ((pawns >> 9) & (~columns[7])) & valid_targets & occupancy_mask_white;
        while (legal_moves) {

            base_move.setTo(bitScanForward(legal_moves));
            base_move.setFrom(base_move.to() + 9);
            legal_moves &= legal_moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            base_move.setTaken(taken);
            if (base_move.to() / 8 == 0) {
                for (size_t i = 2; i < 6; i++)
                {
                    base_move.setUpgrade(i);
                    *move_buffer = base_move;
                    move_buffer++;
                }
                base_move.setUpgrade(0);
            }
            else {
                *move_buffer = base_move;
                move_buffer++;
            }
        }
        legal_moves = ((pawns >> 7) & (~columns[0])) & valid_targets & occupancy_mask_white;
        while (legal_moves) {
            base_move.setTo(bitScanForward(legal_moves));
            base_move.setFrom(base_move.to() + 7);
            legal_moves &= legal_moves - 1;
            u8 taken = mailboard_var[base_move.to()];
            base_move.setTaken(taken);
            if (base_move.to() / 8 == 0) {
                for (size_t i = 2; i < 6; i++)
                {
                    base_move.setUpgrade(i);
                    *move_buffer = base_move;
                    move_buffer++;
                }
                base_move.setUpgrade(0);
            }
            else {
                *move_buffer = base_move;
                move_buffer++;
            }
        }
        legal_moves = ((pawns >> 8)) & (~occupancy_mask);
        mem = legal_moves;
        legal_moves &= valid_targets;
        while (legal_moves) {
            base_move.setTo(bitScanForward(legal_moves));
            base_move.setFrom(base_move.to() + 8);
            u8 taken = 0;
            base_move.setTaken(taken);
            legal_moves &= legal_moves - 1;
            if (base_move.to() / 8 == 0) {
                for (size_t i = 2; i < 6; i++)
                {
                    base_move.setUpgrade(i);
                    *move_buffer = base_move;
                    move_buffer++;
                }
                base_move.setUpgrade(0);
            }
            else {
                *move_buffer = base_move;
                move_buffer++;
            }
        }
        legal_moves = (mem >> 8) & rows[4] & valid_targets & (~occupancy_mask);
        while (legal_moves) {
            base_move.setTo(bitScanForward(legal_moves));
            base_move.setFrom(base_move.to() + 16);
            u8 taken = 0;
            base_move.setTaken(taken);
            *move_buffer = base_move;
            move_buffer++;
            legal_moves &= legal_moves - 1;
        }
        if (ep != 8) {
            u64 all_pawns = bitboard_var[7];
            u64 ep_mask = ONE << (ep + 2 * 8);
            u64 takers = all_pawns & (((ep_mask << 9) & (~columns[0])) | ((ep_mask << 7) & (~columns[7])));
            while (takers) {
                u8 from = bitScanForward(takers);
                takers &= takers - 1;
                //we want to test wheter we can perform the operation and not be in check
                u64 occupancy_mask_sim = (occupancy_mask & (~(ONE << from)) & (~(ep_mask << 8))) | ep_mask;
                u64 king_bishop = bishopMovesReachable(king_index, occupancy_mask_sim);
                u64 king_rook = rookMovesReachable(king_index, occupancy_mask_sim);
                if ((king_bishop & opponent_bishop_like) || (king_rook & opponent_rook_like)) {
                    continue;
                }
                base_move.setFrom(from);
                base_move.setTo(ep + 2 * 8);
                u8 taken = mailboard_var[base_move.to()];
                base_move.setTaken(taken);
                *move_buffer = base_move;
                move_buffer++;
            }
        }
        while (locked_pawns) {
            base_move.setFrom(bitScanForward(locked_pawns));
            locked_pawns &= locked_pawns - 1;
            if (base_move.from() / 8 == king_index / 8) {
                continue;
            }
            else if (base_move.from() % 8 == king_index % 8) {
                //there can be no promotion in this case
                legal_moves = (ONE << (base_move.from() - 8)) & (~occupancy_mask);
                mem = legal_moves;
                legal_moves &= valid_targets;
                while (legal_moves) {
                    base_move.setTo(bitScanForward(legal_moves));
                    base_move.setFrom(base_move.to() + 8);
                    u8 taken = 0;
                    base_move.setTaken(taken);
                    *move_buffer = base_move;
                    move_buffer++;
                    legal_moves &= legal_moves - 1;
                }
                legal_moves = (mem >> 8) & valid_targets & rows[4] & (~occupancy_mask);
                while (legal_moves) {
                    base_move.setTo(bitScanForward(legal_moves));
                    base_move.setFrom(base_move.to() + 16);
                    u8 taken = 0;
                    base_move.setTaken(taken);
                    *move_buffer = base_move;
                    move_buffer++;
                    legal_moves &= legal_moves - 1;
                }
                mem = legal_moves;

            }
            else if (base_move.from() % 8 < king_index % 8) {
                //pawn is to the left and may take to the left. This may cause an upgrade
                legal_moves = (ONE << (base_move.from() - 9)) & occupancy_mask_white & valid_targets;
                while (legal_moves) {
                    base_move.setTo(bitScanForward(legal_moves));
                    legal_moves &= legal_moves - 1;
                    u8 taken = mailboard_var[base_move.to()];
                    base_move.setTaken(taken);
                    if (base_move.to() / 8 == 0) {
                        for (size_t i = 2; i < 6; i++)
                        {
                            base_move.setUpgrade(i);
                            *move_buffer = base_move;
                            move_buffer++;
                        }
                        base_move.setUpgrade(0);
                    }
                    else {
                        *move_buffer = base_move;
                        move_buffer++;
                    }
                }
            }
            else {
                //pawn is to the right and may take to the right. This may cause an upgrade
                legal_moves = (ONE << (base_move.from() - 7)) & occupancy_mask_white & valid_targets;
                while (legal_moves) {
                    base_move.setTo(bitScanForward(legal_moves));
                    legal_moves &= legal_moves - 1;
                    u8 taken = mailboard_var[base_move.to()];
                    base_move.setTaken(taken);
                    if (base_move.to() / 8 == 0) {
                        for (size_t i = 2; i < 6; i++)
                        {
                            base_move.setUpgrade(i);
                            *move_buffer = base_move;
                            move_buffer++;
                        }
                        base_move.setUpgrade(0);
                    }
                    else {
                        *move_buffer = base_move;
                        move_buffer++;
                    }
                }
            }
        }
    }
    if (move_buffer == move_buffer_start) {
        if (checks) {
            *move_buffer = Move();
        }
        else {
            *move_buffer = Move();
            (*move_buffer).setFrom(1);
        }
    }
    return move_buffer;
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
        if ((*move_start).taken() == 6) {
            std::cout << BoardConversions::bbToFenString(board);
        }
        
        board.make(*move_start);
        res += _perftHelp(board, depth - 1, end);
        board.unmake(*move_start);
        move_start++;
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
    Move moves[1000];
    return _perftHelp(board, depth, moves);
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
        move_start++;
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
    Move moves[1000];
    return _perftLeafHelp(board, depth, moves);
};