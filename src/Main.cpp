// Magnificence Chess.cpp : Defines the entry point for the console application.
/**
 * @file Main.cpp
 * @author William Sandström and Harald Bjurulf
 * @brief Entry point for application.
 * @version 0.1
 * @date 2020-05-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <iostream>

#include "Board/BitBoard.h"
#include "Board/tests_bitboard.h"
#include "Interface/Interface.h"
#include "settings.h"

 /// @brief Runs tests if in DEBUG and executes the main command loop by creating the Interface class.
int main()
{
    #if defined(DEBUG)
        std::cout << "Running tests..." << std::endl;
        run_tests_bitboard();
        std::cout << "Tests passed\n" << std::endl;
    #endif

	Interface interface;
    interface.run();

}
