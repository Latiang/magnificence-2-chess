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

#include <string>
#include <vector>
#include <random>
#include <memory>
#if defined(DEBUG)
    #include <cassert>
#endif

#include "Move.h"
#include "type_definitions.h"

const bool WHITE = true;
const bool BLACK = false;

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
        void removePiece(size_t index);

        /**
         * @brief Adds the given piece at index to the board
         * 
         * @param index 
         * @param piece 
         */
        void addPiece(size_t index, u8 piece);
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
        std::string fenString();

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
        Move * moveGenWhite(Move *move_start_buffer) {

        }

        /**
         * @brief Generates legal moves for black
         * 
         * @param move_start_buffer moves will be inserted with start here and new moves will be written to following adresses
         * @return Move* returns adress after the last move inserted
         */
        Move * moveGenBlack(Move *move_start_buffer) {

        }

        /**
         * @brief Generates legal moves for the current player
         * 
         * @param move_start_buffer moves will be inserted with start here and new moves will be written to following adresses
         * @return Move* returns adress after the last move inserted
         */

        Move * moveGen(Move *move_start_buffer) {
            
        }

        /**
         * @brief returns the player to_move, true for white, false for black
         * 
         * @return Bool 
         */
        bool toMove() {
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
u64 _perftHelp(BitBoard &board, u64 depth, Move *move_start);


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
u64 _perftLeafHelp(BitBoard &board, u64 depth, Move *move_start);

/**
 * @brief Peforms perft for given position with leaf node optimizations
 * 
 * @param board 
 * @param depth
 * @return u64 
 */

u64 perftLeaf(BitBoard &board, u64 depth);

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
