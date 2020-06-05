/**
 * @file StringHelpers.h
 * @author William Sandström and Harald Bjurulf
 * @brief This file contains the static StringHelpers class and the StringArguments class
 * @version 0.1
 * @date 2020-05-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cctype>
#include <ctype.h>

#include "../Board/BitBoard.h"

/**
 * @brief  A static helper class which is used for various common string tasks, such as string splitting.
 */
class StringHelpers
{
public:
    static std::vector<std::string> splitString(std::string string, char delimiter, bool transform_to_lowercase);
    static int extractFirstNumber(std::string string);
private:
    //Static class, constructors private
    StringHelpers() {};
    ~StringHelpers() {};
};

/**
 * @brief A class which is used to represents command inputs by splitting them up in parts for easier access.
 */
struct StringArguments
{
    StringArguments(std::string input);
    //The first word of a command input, the 'command'
    std::string command = "None";
    //The rest of the words of a command input, the 'arguments'
    std::vector<std::string> arguments;
    //Full argument is saved here incase it is needed by some command
    std::string arguments_string = "None";

    std::string isolateFenString(int argument_start_index);
    int getNamedArgument(std::string argument_name, int error_value = -1);
};