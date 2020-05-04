// Magnificence Chess.cpp : Defines the entry point for the console application.
//
#define DEBUG 1
#include <iostream>

#include "Board/BitBoard.h"
#include "Board/tests_bitboard.h"
#include "Interface/Interface.h"

int main()
{

    #if DEBUG
        std::cout << "Running tests..." << std::endl;
        run_tests_bitboard();
        std::cout << "Tests passed\n" << std::endl;
    #endif

	Interface interface;
    interface.run();

}
