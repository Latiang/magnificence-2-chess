#pragma once
#include "type_definitions.h"

/**
 * @brief represents a chess move and stores the necessary data to unmake the move
 * 
 */
const u64 ONE = 1;
const u64 FULL = -1;

struct Move {
    private:
        u32 data;
        const u8 TAKEN_INDEX = 29;
        const u64 TAKEN_MASK = ((u64)0b111) << 29;
    public:
        /*
            storage plan
            taken piece, 0..5, 3 bits
            ep square    0..8  4 bits
            castling           4 bits
            from               6 bits
            to                 6 bits
            total             23 bits
            
        */
        /**
         * @brief Sets ein passant column for the current board state
         * 
         * @param new_ep the new ein passant column, 8 for no ep
         */
        void set_ep(u8 new_ep) {
            
        }
        /**
         * @brief Returns the current ein passant column
         * 
         * @return u8 0..8, 8 for no ep
         */
        u8 ep() {
            return 0;
        };
        

};

