/**
 * @file tests_bitboard.h
 * @author Harald Bjurulf (haraldbjurulf@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-05-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */
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

/**
 * @brief Performs unit tests on the bitboard type
 * 
 */
void test_bitboard() {
    BitBoard board;
    Move *moves = new Move[10000];
    std::string fen_mem = board.fen_string();
    u64 hash_mem = board.hash();
    Move *end = board.move_gen_w(moves);
    Move move = *(moves);
    assert(board.to_move() == true);
    board.make(move);
    assert(board.to_move() == false);
    board.move_gen_b(moves);
    board.make(*moves);
    assert(board.to_move() == true);
    board.unmake(*moves);
    assert(board.to_move() == false);
    board.unmake(move);
    assert(board.to_move() == true);
    assert(board.fen_string() == fen_mem);
    assert(board.hash() == hash_mem);
    std::string positions[] = { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                             "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
                             "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",
                             "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
                             "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",
                             "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
                             "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10" };
    std::vector<u64> results[] = { std::vector<u64>(), std::vector<u64>(), std::vector<u64>(), std::vector<u64>(), std::vector<u64>(), std::vector<u64>(), std::vector<u64>() };
    results[0].push_back(20);
    results[0].push_back(400);
    results[0].push_back(8902);
    results[0].push_back(197281);
    results[0].push_back(4865609);
    results[0].push_back(119060324);

    results[1].push_back(48);
    results[1].push_back(2039);
    results[1].push_back(97862);
    results[1].push_back(4085603);
    results[1].push_back(193690690);
    results[1].push_back(8031647685);

    results[2].push_back(14);
    results[2].push_back(191);
    results[2].push_back(2812);
    results[2].push_back(43238);
    results[2].push_back(674624);
    results[2].push_back(11030083);
    results[2].push_back(178633661);
    results[2].push_back(3009794393);

    results[3].push_back(6);
    results[3].push_back(264);
    results[3].push_back(9467);
    results[3].push_back(422333);
    results[3].push_back(15833292);
    results[3].push_back(706045033);

    results[4].push_back(6);
    results[4].push_back(264);
    results[4].push_back(9467);
    results[4].push_back(422333);
    results[4].push_back(15833292);
    results[4].push_back(706045033);

    results[5].push_back(44);
    results[5].push_back(1486);
    results[5].push_back(62379);
    results[5].push_back(2103487);
    results[5].push_back(89941194);

    results[6].push_back(46);
    results[6].push_back(2079);
    results[6].push_back(89890);
    results[6].push_back(3894594);
    results[6].push_back(164075551);
    results[6].push_back(6923051137);
    results[6].push_back(287188994746);
    results[6].push_back(11923589843526);

    for (size_t i = 0; i < 7; i++)
    {
        board = BitBoard(positions[i]);
        fen_mem = board.fen_string();
        hash_mem = board.hash();
        for (size_t j = 0; j < results[i].size(); j++)
        {
            assert(perft(board, j+1) == results[i][j]);
            assert(board.fen_string() == fen_mem);
            assert(board.hash() == hash_mem);
        }
        
    }
    
    delete[] moves;
}

int run_tests_bitboard() {
    std::cout << "Running bitboard.h unit tests" << std::endl;
    test_move();
    return 0;
}