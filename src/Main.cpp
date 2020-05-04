// Magnificence Chess.cpp : Defines the entry point for the console application.
//
#pragma once
#define DEBUG 1
#include <iostream>
#include "BitBoard.h"
#if DEBUG
    #include "tests_bitboard.h"
#endif
int main()
{
    #if DEBUG
        run_tests_bitboard();
    #endif
	std::cout << "Test" << std::endl;
    return 0;
}
