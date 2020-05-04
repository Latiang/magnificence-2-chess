#pragma once
#include <iostream>
#include "BitBoard.h"
#include <cstdint>
#include <cassert>


/**
 * @brief performs unit tests on the move type
 * 
 */
void test_move() {
    Move test_variable;
    for (size_t i = 0; i < 51; i++)
    {
        test_variable.set_silent(i);
        assert (i == test_variable.silent());
    }
    for (size_t i = 0; i < 9; i++)
    {
        test_variable.set_ep(i);
        assert (i == test_variable.ep());
    }
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 64; i++)
    {
        test_variable.set_from(i);
        assert(i == test_variable.from());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 64; i++)
    {
        test_variable.set_to(i);
        assert(i == test_variable.to());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 16; i++)
    {
        test_variable.set_castling(i);
        assert(i == test_variable.castling());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 7; i++)
    {
        test_variable.set_taken(i);
        assert(i == test_variable.taken());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 6; i++)
    {
        test_variable.set_upgrade(i);
        assert(i == test_variable.upgrade());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.silent() == 50);

    
    for (size_t i = 0; i < 7; i++)
    {
        test_variable.set_taken(i);
        assert(i == test_variable.taken());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 16; i++)
    {
        test_variable.set_castling(i);
        assert(i == test_variable.castling());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 64; i++)
    {
        test_variable.set_to(i);
        assert(i == test_variable.to());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 64; i++)
    {
        test_variable.set_from(i);
        assert(i == test_variable.from());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 9; i++)
    {
        test_variable.set_ep(i);
        assert (i == test_variable.ep());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
    assert(test_variable.silent() == 50);
    for (size_t i = 0; i < 51; i++)
    {
        test_variable.set_silent(i);
        assert (i == test_variable.silent());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
    assert(test_variable.silent() == 50);
}


int run_tests_bitboard() {
    std::cout << "Running bitboard.h unit tests" << std::endl;
    test_move();
    return 0;
}