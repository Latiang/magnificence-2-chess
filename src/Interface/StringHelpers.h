#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

class StringHelpers
{
public:
    static std::vector<std::string> splitString(std::string string, char delimiter, bool transformToLowerCase);
private:
    //Static class
    StringHelpers() {};
    ~StringHelpers() {};
};


struct StringArguments
{
    StringArguments(std::string input);
    std::string command = "None";
    std::vector<std::string> arguments;
};