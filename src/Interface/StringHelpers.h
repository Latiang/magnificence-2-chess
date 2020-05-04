#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cctype>

#include "../Board/BitBoard.h"

class StringHelpers
{
public:
    static std::vector<std::string> splitString(std::string string, char delimiter, bool transformToLowerCase);
    static std::string getDisplayString(BitBoard& board);
private:
    //Static class
    static char pieceToChar(int piece);

    StringHelpers() {};
    ~StringHelpers() {};
};


struct StringArguments
{
    StringArguments(std::string input);
    std::string command = "None";
    std::string argumentsString = "None";
    std::vector<std::string> arguments;
};