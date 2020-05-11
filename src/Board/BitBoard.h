/**
 * @file BitBoard.h
 * @author Harald Bjurulf (haraldbjurulf@hotmail.com)
 * @brief Defines move and bitboard types
 * @version 0.1
 * @date 2020-05-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#if defined(_WIN32)
#include <intrin.h>
#elif defined(__gnu_linux__) || defined(__linux__) || defined(__CYGWIN__)
#include "x86intrin.h"
#endif
#include "type_definitions.h"
#include <string>
#include <vector>
#include <random>
#include <memory>
#if defined(DEBUG)
    #include <cassert>
#endif

const bool WHITE = true;
const bool BLACK = false;

const u64 ONE = 1;
const u64 FULL = -1;
const u8 UPGRADE_INDEX = 29;
const u64 UPGRADE_MASK = ((u64)0b111) << UPGRADE_INDEX;
const u8 TAKEN_INDEX = 26;
const u64 TAKEN_MASK = ((u64)0b111) << TAKEN_INDEX;
const u8 FROM_INDEX = 0;
const u64 FROM_MASK = ((u64)0b111111) << FROM_INDEX;
const u8 TO_INDEX = 6;
const u64 TO_MASK = ((u64)0b111111) << TO_INDEX;
const u8 EP_INDEX = 12;
const u64 EP_MASK = ((u64)0b1111) << EP_INDEX;
const u8 CASTLING_INDEX = 16;
const u64 CASTLING_MASK = ((u64)0b1111) << CASTLING_INDEX;
/**
 * @brief represents a chess move and stores the necessary data to unmake the move
 * 
 */
struct Move {
    private:
        u32 data;
    public:
        //These functions are probably small and will therefore be defined here to improve inlining(?)
        /*
            storage plan
            upgrade      0..6  3 bits x
            taken piece, 0..5, 3 bits x
            ep square    0..8  4 bits x
            castling           4 bits x
            from               6 bits x
            to                 6 bits x
            silent             6 bits
            total             32 bits
            plan for storage
            upgrade     u
            Taken       t
            from        f
            to          T
            ep          e
            castling    c
            uuut tt00 0000 cccc eeee TTTT TTff ffff 
              28   24   20   16   12    8    4    0
        */

        /**
         * @brief Sets ein passant column for the current board state
         * 
         * @param ep the new ein passant column, 8 for no ep
         */
        void set_ep(u8 ep) {
            #if DEBUG
                assert (ep <= 8);
            #endif
            this->data = (this->data & (~EP_MASK)) | (((u64)ep) << EP_INDEX);
        }

        /**
         * @brief Construct an empty move
         * 
         */
        Move() {
            this->data = 0;
        }

        /**
         * @brief Returns the current ein passant column
         * 
         * @return u8 in [0, 8]
         */
        u8 ep() {
            return (u8)((this->data & EP_MASK) >> EP_INDEX);
        }
        
        /**
         * @brief Set the from square
         * 
         * @param from the index of the square, must be in [0, 63]
         */
        void set_from(u8 from) {
            #if DEBUG
                assert (from <= 63);
            #endif
            this->data = (this->data & (~FROM_MASK)) | (((u64)from) << FROM_INDEX);
        }

        /**
         * @brief returns the from square index
         * 
         * @return u8 in [0,63]
         */
        u8 from() {
            return (u8)((this->data & FROM_MASK) >> FROM_INDEX);
        }

        /**
         * @brief Set the to square
         * 
         * @param to the index of the to square, must be in [0, 63]
         */
        void set_to(u8 to) {
            #if DEBUG
                assert (to <= 63);
            #endif
            this->data = (this->data & (~TO_MASK)) | (((u64)to) << TO_INDEX);
        }

        /**
         * @brief Returns the to square index
         * 
         * @return u8 in [0, 63]
         */
        u8 to() {
            return (u8)((this->data & TO_MASK) >> TO_INDEX);
        }

        /**
         * @brief Set the castling rights
         * 
         * @param castling_rights 4 bit number with bits corresponding to castling rights
         */
        void set_castling(u8 castling_rights) {
            #if DEBUG
                assert (castling_rights < 16);
            #endif
            this->data = (this->data & (~CASTLING_MASK)) | (((u64)castling_rights) << CASTLING_INDEX);
        }

        /**
         * @brief Returns the 4 bit representation of the castling rights
         * 
         * @return u8 in [0, 15]
         */
        u8 castling() {
            return (u8)((this->data & CASTLING_MASK) >> CASTLING_INDEX);
        }

        /**
         * @brief Set the taken piece
         * 
         * @param taken_piece numbers 0..6 corespond to empty, pawn, bishop, knight, rook, queen, king
         */
        void set_taken(u8 taken_piece) {
            #if DEBUG
                assert(taken_piece < 7);
            #endif
            this->data = (this->data & (~TAKEN_MASK)) | (((u64)taken_piece) << TAKEN_INDEX);
        }
        /**
         * @brief Return the taken piece
         * 
         * @return u8 in [0, 6]
         */
        u8 taken() {
            return (u8)((this->data & TAKEN_MASK) >> TAKEN_INDEX);
        }

        /**
         * @brief Set the upgrade target piece
         * 
         * @param upgrade numbers 0..5 correspond to no upgrade, pawn, bishop, knight, rook, queen
         */
        void set_upgrade(u8 upgrade) {
            #if DEBUG
                assert(upgrade < 6);
            #endif
            this->data = (this->data & (~UPGRADE_MASK)) | (((u64)upgrade) << UPGRADE_INDEX);
        }

        /**
         * @brief Return the upgrade target piece
         * 
         * @return u8 in [0, 5]
         */
        u8 upgrade() {
            return (u8)((this->data & UPGRADE_MASK) >> UPGRADE_INDEX);
        }
};

/**
 * @brief Wraps a vector to simplify move generation
 * 
 */



/**
 * @brief The basic data for a bitboard
 * 
 */
struct BitBoardBase {
    public:
        /**
         * @brief 0: empty, 1: wPawn, 2: wKnight, 3:wBishop, 4: wRook, 5: wQueen, 6: wKing, 7: bPawn, 8: bKnight, 9: bBishop, 10: bRook, 11: bQueen, 12: bKing
         * 
         */
        u64 pieces [13];
};

/**
 * @brief The basic data for a mail board
 * 
 */
struct MailBoardBase {
    public:
        /**
         * @brief The index 0 maps to a1, 1 to b1, ..., 8 to a2, 9 to b2, and so forth. Pieces have values according to 0: empty, 1: wPawn, 2: wKnight, 3:wBishop, 4: wRook, 5: wQueen, 6: wKing, 7: bPawn, 8: bKnight, 9: bBishop, 10: bRook, 11: bQueen, 12: bKing
         * 
         */
        u8 pieces[64];
};

/**
 * @brief A complete chess board implementation including make and unmake move
 * 
 */
class BitBoard {
    private:
        /**
         * @brief The bit tables for the pieces
         * 
         */
        BitBoardBase bitboard_var;
        /**
         * @brief The mailboard representation of the board
         * 
         */
        MailBoardBase mailboard_var;
        /**
         * @brief Number of silent moves in a row
         * 
         */
        u8 silent;
        /**
         * @brief 4 bit number showing castling rights. White King side index 3, white queen side 2, black king side 1, black queen side 0
         * 
         */
        u8 castling;
        /**
         * @brief stores the EP column of the board
         * 
         */
        u8 ep;
        /**
         * @brief true for white, false for black
         * 
         */
        bool color;
        /**
         * @brief The zoobrist key
         * 
         */
        u64 zoobrist;
        /**
         * @brief Used to reset silent moves for make unmakes
         * 
         */
        std::vector<u8> silent_mem;
        /**
         * @brief Removes the piece at the specified index
         * 
         * @param index 
         */
        void remove_piece(size_t index);

        /**
         * @brief Adds the given piece at index to the board
         * 
         * @param index 
         * @param piece 
         */
        void add_piece(size_t index, u8 piece);
    public:        
        /**
         * @brief Creates a new BitBoard in the startposition
         * 
         */
        BitBoard();

        /**
         * @brief Copies the given bitboard
         * 
         * @param original 
         */
        BitBoard(const BitBoard &original);

        /**
         * @brief Creates a board from a fen_string
         * 
         * @param fen_string 
         */
        BitBoard(const std::string &fen_string);

        /**
         * @brief Creates and returns the fen string of the current position
         * 
         * @return std::string
         */
        std::string fen_string() {

        }

        /**
         * @brief Returns the bitboard representation of the board
         * 
         * @return const BitBoardBase& 
         */
        const BitBoardBase &bitboard() {
            return this->bitboard_var;
        };

        /**
         * @brief Returns the mailboard representation of the board
         * 
         * @return const MailBoardBase& 
         */
        const MailBoardBase &mailboard() {
            return this->mailboard_var;
        };

        /**
         * @brief Makes the given move
         * 
         * @param move 
         */
        void make(Move move);

        /**
         * @brief Unmakes the given move
         * 
         * @param move 
         */
        void unmake(Move move);

        /**
         * @brief Generates legal moves for white
         * 
         * @param move_start_buffer moves will be inserted with start here and new moves will be written to following adresses
         * @return Move* returns adress after the last move inserted
         */
        Move * move_gen_w(Move *move_start_buffer) {

        }

        /**
         * @brief Generates legal moves for black
         * 
         * @param move_start_buffer moves will be inserted with start here and new moves will be written to following adresses
         * @return Move* returns adress after the last move inserted
         */
        Move * move_gen_b(Move *move_start_buffer) {

        }

        /**
         * @brief Generates legal moves for the current player
         * 
         * @param move_start_buffer moves will be inserted with start here and new moves will be written to following adresses
         * @return Move* returns adress after the last move inserted
         */
        Move * move_gen(Move *move_start_buffer) {

        }

        /**
         * @brief returns the player to_move, true for white, false for black
         * 
         * @return Bool 
         */
        bool to_move() {
            return color;
        }

        /**
         * @brief Returns the zoobrist hash of the board
         * 
         * @return u64 
         */
        u64 hash() {
            return zoobrist;
        }
};

/**
 * @brief recursively calculates perft for given board and depth
 * 
 * @param board 
 * @param depth 
 * @param move_start 
 * @return u64 
 */
u64 _perft_help(BitBoard &board, u64 depth, Move *move_start);


/**
 * @brief Peforms perft for given position without leaf node optimizations
 * 
 * @param board 
 * @param depth
 * @return u64 
 */
u64 perft(BitBoard &board, u64 depth);
/**
 * @brief recursively calculates perft for given board and depth
 * 
 * @param board 
 * @param depth 
 * @param move_start 
 * @return u64 
 */
u64 _perft_leaf_help(BitBoard &board, u64 depth, Move *move_start);

/**
 * @brief Peforms perft for given position with leaf node optimizations
 * 
 * @param board 
 * @param depth
 * @return u64 
 */
u64 perft_leaf(BitBoard &board, u64 depth);

/**
 * @brief Returns the number of set bits in value
 * 
 * @param value
 * @return u8 
 */
inline u8 populationCount(const u64 value)
{
    #if defined(_WIN32)
    return (u8)__popcnt64(value);
    #elif defined(__gnu_linux__) || defined(__linux__) || defined(__CYGWIN__)
    return (u8)__builtin_popcountll(value);
    #endif
}

/**
 * @brief Returns the index of the lowest set bit in piece
 * 
 * @param piece 
 * @return u32 
 */
inline u32 bitScanForward(const u64 piece)
{
    #if defined(_WIN32)
    unsigned long int index;
    _BitScanForward64(&index, piece);
    return index;

    #elif defined(__gnu_linux__) || defined(__linux__) || defined(__CYGWIN__)
    return __builtin_ctzll(piece);
    #endif
}

/**
 * @brief Returns the bits from occupancy corrsponding to the set bits of mask at the end of the returned integer value
 * 
 * @param occupancy 
 * @param mask 
 * @return u64 
 */
inline u64 pext(u64 occupancy, u64 mask)
{
    return _pext_u64(occupancy, mask);
}