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
            stop_mask &= variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem << 7;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //up right
            stop_mask = ~(columns[7] | rows[7]);
            stop_mask &= variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem << 9;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //down left
            stop_mask = ~(columns[0] | rows[0]);
            stop_mask &= variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem >> 9;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //down right
            stop_mask = ~(columns[7] | rows[0]);
            stop_mask &= variation;
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
            stop_mask &= variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem << 8;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //right
            stop_mask = ~(columns[7]);
            stop_mask &= variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem << 1;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //down
            stop_mask = ~(rows[0]);
            stop_mask &= variation;
            mem = position & stop_mask; //the starting position is such that there are no moves
            while (mem) {
                mem = mem >> 8;
                options |= mem;
                mem &= stop_mask; //figure out if the position is now on a stop_mask
            }
            //left
            stop_mask = ~(columns[0]);
            stop_mask &= variation;
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
    u8 color_mod = (color) * 6;
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
 * @brief Creates and returns the fen string of the current position
 * 
 * @return std::string
 */
std::string BitBoard::fenString()
{
	return "fen";
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
};
