 #include "StringHelpers.h"
 
/// @brief Split the string using delimiter and, if asked, turn all characters to lowercase
std::vector<std::string> StringHelpers::splitString(std::string input, char delimiter, bool transformToLowerCase = true)
{
    //Split the string and turn all characters to lowercase
	std::vector<std::string> output;
	size_t foundSymbolAt = 0;
	size_t foundSymbolAtOld = -1;
	int i = 0;
	while (foundSymbolAt != -1)
	{
		foundSymbolAt = input.find(delimiter, (int)(foundSymbolAtOld + 1));
		output.push_back(input.substr(foundSymbolAtOld + 1, foundSymbolAt - foundSymbolAtOld - 1));
		foundSymbolAtOld = foundSymbolAt;
		i++;
	}

    //Transform the output vector of strings into lowercase
    if (transformToLowerCase)
    {
        for (auto &word: output)
        {
            std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c){ return std::tolower(c); });
        }
    }

	return output;
}

/// @brief Initilization of String Arguments with an input string. Split up the input, assign the first item as the command, the rest as arguments
StringArguments::StringArguments(std::string input)
{
    std::vector<std::string> argumentVector = StringHelpers::splitString(input, ' ');
    argumentsString = std::accumulate(argumentVector.begin(), argumentVector.end(), std::string(""));
    if (argumentVector.size() > 0)
    {
        command = argumentVector[0];
        argumentVector.erase(argumentVector.begin());
        this->arguments = argumentVector;
    }
}

/// @brief returns the complete fen string which starts at argumentStartIndex
 std::string StringArguments::isolateFenString(int argumentStartIndex)
 {
    std::string fen = arguments[argumentStartIndex] + " " + arguments[argumentStartIndex+1] + " " + arguments[argumentStartIndex+2]
				+ " " + arguments[argumentStartIndex+3] + " " + arguments[argumentStartIndex+4] + " " + arguments[argumentStartIndex+5];
    return fen;
 }

/// @brief returns the argument integer for a specific argument name. Ex command "go depth 1", "depth" will return 1.
/// Returns errorValue if argument is not found
int StringArguments::getNamedArgument(std::string argumentName, int errorValue)
 {
    for (size_t i = 0; i < arguments.size(); i++)
    {
        if (arguments[i] == argumentName &&  i+1 < arguments.size()) //If the argument matches, return the next argument (the value)
            return std::stoi(arguments[i+1]);
    }
    return errorValue;
 }