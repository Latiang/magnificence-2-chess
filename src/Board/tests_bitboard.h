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
#include "Move.h"
#include "../Interface/BoardConversions.h"
#include <cstdint>
#include <cassert>


/**
 * @brief performs unit tests on the move type
 * 
 */
void test_move() {
    Move test_variable;
    for (size_t i = 0; i < 9; i++)
    {
        test_variable.setEP(i);
        assert (i == test_variable.ep());
    }
    for (size_t i = 0; i < 64; i++)
    {
        test_variable.setFrom(i);
        assert(i == test_variable.from());
    }
    assert(test_variable.ep() == 8);
    for (size_t i = 0; i < 64; i++)
    {
        test_variable.setTo(i);
        assert(i == test_variable.to());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    for (size_t i = 0; i < 16; i++)
    {
        test_variable.setCastling(i);
        assert(i == test_variable.castling());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    for (size_t i = 0; i < 7; i++)
    {
        test_variable.setTaken(i);
        assert(i == test_variable.taken());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    for (size_t i = 0; i < 6; i++)
    {
        test_variable.setUpgrade(i);
        assert(i == test_variable.upgrade());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);

    
    for (size_t i = 0; i < 7; i++)
    {
        test_variable.setTaken(i);
        assert(i == test_variable.taken());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
    for (size_t i = 0; i < 16; i++)
    {
        test_variable.setCastling(i);
        assert(i == test_variable.castling());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
     for (size_t i = 0; i < 64; i++)
    {
        test_variable.setTo(i);
        assert(i == test_variable.to());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
     for (size_t i = 0; i < 64; i++)
    {
        test_variable.setFrom(i);
        assert(i == test_variable.from());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
     for (size_t i = 0; i < 9; i++)
    {
        test_variable.setEP(i);
        assert (i == test_variable.ep());
    }
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
    assert(test_variable.ep() == 8);
    assert(test_variable.from() == 63);
    assert(test_variable.to() == 63);
    assert(test_variable.castling() == 15);
    assert(test_variable.taken() == 6);
    assert(test_variable.upgrade() == 5);
 }

/**
 * @brief Performs unit tests on the bitboard type
 * 
 */
void test_bitboard() {
    BitBoardBase bb;
    bb[1] = 5;
    assert(bb[1] == 5);
    bb[2] = 3;
    assert(bb[2] == 3);
    assert(bb[1] == 5);
    bb[1] = 8;
    assert(bb[1] == 8);
    assert(bb[2] == 3);
    MailBoardBase mb;
    mb[1] = 5;
    assert(mb[1] == 5);
    mb[2] = 3;
    assert(mb[2] == 3);
    assert(mb[1] == 5);
    mb[1] = 8;
    assert(mb[1] == 8);
    assert(mb[2] == 3);
    BitBoard board;
    Move *moves = new Move[10000];
    std::string fen_mem = BoardConversions::bbToFenString(board);
    u64 hash_mem = board.hash();
    Move *end = board.moveGenWhite(moves);
    Move move = *(moves);
    assert(board.toMove() == true);
    board.make(move);
    assert(board.toMove() == false);
    board.moveGenBlack(moves);
    board.make(*moves);
    assert(board.toMove() == true);
    board.unmake(*moves);
    assert(board.toMove() == false);
    board.unmake(move);
    assert(board.toMove() == true);
    assert(BoardConversions::bbToFenString(board) == fen_mem);
    assert(board.hash() == hash_mem);
    std::string positions[] = { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                             "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
                             "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
                             "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
                             "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1",
                             "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
                             "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
                             "3k4/4bppp/p1Kp4/5b2/Q3PB2/6P1/PPP2nqP/4R3 w - - 1 1",
                             "4r3/ppp2NQp/6p1/q3pb2/5B2/P1kP4/4BPPP/3K4 b - - 0 1" };
    std::vector<std::vector<u64>> results;
    results.resize(9);
    //Starting position
    results[0] = {20, 400, 8902, 197281, 4865609, 119060324};

    //Kiwipete
    results[1] = {48, 2039, 97862, 4085603, 193690690};
    //results[1].push_back(8031647685); //Takes a long time

    //EP test
    results[2] = {14, 191, 2812, 43238, 674624, 11030083};

    results[3] = {6, 264, 9467, 422333, 15833292, 706045033};

    //Same as three but mirrored
    results[4] = {6, 264, 9467, 422333, 15833292, 706045033};

    results[5] = {44, 1486, 62379, 2103487, 89941194};

    results[6] = {46, 2079, 89890, 3894594, 164075551};
    //results[6].push_back(6923051137); //Takes a long time

    results[7] = { 35, 1150, 38100, 1240346 };

    results[8] = results[7];

    for (size_t i = 0; i < results.size(); i++)
    {
        std::cout << "Running perft test position " << i+1 << "/" << results.size() << "\n";
        board = BitBoard(positions[i]);
        fen_mem = BoardConversions::bbToFenString(board);
        hash_mem = board.hash();
        for (size_t j = 0; j < results[i].size(); j++)
        {
            assert(perftLeaf(board, j + 1) == results[i][j]);
            assert(BoardConversions::bbToFenString(board) == fen_mem);
            assert(board.hash() == hash_mem);
        }
        
    }
    
    delete[] moves;
}

int run_tests_bitboard() {
    std::cout << "Running bitboard.h unit tests" << std::endl;
    test_move();
    //test_bitboard();
    std::cout << "bitboard.h unit tests complete" << std::endl;
    return 0;
}