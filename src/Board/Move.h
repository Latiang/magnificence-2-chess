#pragma once


#if defined(DEBUG)
#include <cassert>
#endif
#include "type_definitions.h"

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
         * @brief Sets en passant column for the current board state
         * 
         * @param ep the new en passant column, 8 for no ep
         */
        inline void setEP(u8 ep) {
            #if defined(DEBUG)
                assert (ep <= 8);
            #endif
            this->data = (this->data & (~EP_MASK)) | (((u64)ep) << EP_INDEX);
        }

        /**
         * @brief Construct an empty move
         * 
         */
        inline Move() {
            this->data = 0;
        }

        /**
         * @brief Returns the current en passant column
         * 
         * @return u8 in [0, 8]
         */
        inline u8 ep() {
            return (u8)((this->data & EP_MASK) >> EP_INDEX);
        }
        
        /**
         * @brief Set the from square
         * 
         * @param from the index of the square, must be in [0, 63]
         */
        inline void setFrom(u8 from) {
            #if defined(DEBUG)
                assert (from <= 63);
            #endif
            this->data = (this->data & (~FROM_MASK)) | (((u64)from) << FROM_INDEX);
        }

        /**
         * @brief returns the from square index
         * 
         * @return u8 in [0,63]
         */
        inline u8 from() {
            return (u8)((this->data & FROM_MASK) >> FROM_INDEX);
        }

        /**
         * @brief Set the to square
         * 
         * @param to the index of the to square, must be in [0, 63]
         */
        inline void setTo(u8 to) {
            #if defined(DEBUG)
                assert (to <= 63);
            #endif
            this->data = (this->data & (~TO_MASK)) | (((u64)to) << TO_INDEX);
        }

        /**
         * @brief Returns the to square index
         * 
         * @return u8 in [0, 63]
         */
        inline u8 to() {
            return (u8)((this->data & TO_MASK) >> TO_INDEX);
        }

        /**
         * @brief Set the castling rights
         * 
         * @param castling_rights 4 bit number with bits corresponding to castling rights
         */
        inline void setCastling(u8 castling_rights) {
            #if defined(DEBUG)
                assert (castling_rights < 16);
            #endif
            this->data = (this->data & (~CASTLING_MASK)) | (((u64)castling_rights) << CASTLING_INDEX);
        }

        /**
         * @brief Returns the 4 bit representation of the castling rights
         * 
         * @return u8 in [0, 15]
         */
        inline u8 castling() {
            return (u8)((this->data & CASTLING_MASK) >> CASTLING_INDEX);
        }

        /**
         * @brief Set the taken piece
         * 
         * @param taken_piece numbers 0..6 corespond to empty, pawn, bishop, knight, rook, queen, king
         */
        inline void setTaken(u8 taken_piece) {
            #if defined(DEBUG)
                assert(taken_piece < 7);
            #endif
            this->data = (this->data & (~TAKEN_MASK)) | (((u64)taken_piece) << TAKEN_INDEX);
        }
        /**
         * @brief Return the taken piece
         * 
         * @return u8 in [0, 6]
         */
        inline u8 taken() {
            return (u8)((this->data & TAKEN_MASK) >> TAKEN_INDEX);
        }

        /**
         * @brief Set the upgrade target piece
         * 
         * @param upgrade numbers 0..5 correspond to no upgrade, pawn (NOT ALLOWED), knight, bishop, rook, queen
         */
        inline void setUpgrade(u8 upgrade) {
            #if defined(DEBUG)
                assert(upgrade < 6);
            #endif
            this->data = (this->data & (~UPGRADE_MASK)) | (((u64)upgrade) << UPGRADE_INDEX);
        }

        /**
         * @brief Return the upgrade target piece
         * 
         * @return u8 in [0, 5]
         */
        inline u8 upgrade() {
            return (u8)((this->data & UPGRADE_MASK) >> UPGRADE_INDEX);
        }
        
        inline bool isCapture()
        {
            return (taken() != 0);
        }

        inline u32 getData()
        {
            return data;
        }

        /// @brief Rotate the move positions as if you were switching color
        void rotateForColor()
        {
            setTo(to() % 8 + (7 - to() / 8) * 8);
            setFrom(from() % 8 + (7 - from() / 8) * 8);
        }
};
