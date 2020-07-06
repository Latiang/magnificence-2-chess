 #include "StringHelpers.h"
 
/// @brief Split the string using delimiter and, if asked, turn all characters to lowercase
std::vector<std::string> StringHelpers::splitString(std::string input, char delimiter, bool transform_to_lowercase = false)
{
    //Split the string and turn all characters to lowercase
	std::vector<std::string> output;
	size_t found_symbol_at = 0;
	size_t found_symbol_at_old = -1;
	int i = 0;
	while (found_symbol_at != -1)
	{
		found_symbol_at = input.find(delimiter, (int)(found_symbol_at_old + 1));
        std::string argument = input.substr(found_symbol_at_old + 1, found_symbol_at - found_symbol_at_old - 1);
        if (argument != "" || argument != "")
		    output.push_back(input.substr(found_symbol_at_old + 1, found_symbol_at - found_symbol_at_old - 1));
		found_symbol_at_old = found_symbol_at;
		i++;
	}

    //Transform the output vector of strings into lowercase
    if (transform_to_lowercase)
    {
        for (auto &word: output)
        {
            std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c){ return std::tolower(c); });
        }
    }

	return output;
}

/// @brief Extract the first number which occurs in the string. Does no error checking, so it will only work for a string with an actual number.
int StringHelpers::extractFirstNumber(std::string string)
{
    //Split the string and turn all characters to lowercase
	std::string number = "";
    bool found_number = false;
	for (size_t i = 0; i < string.length(); i++)
    {
        if (std::isdigit(string[i]))
        {
            number += string[i];
            found_number = true;
        }
        else if (found_number) //End of number, quit
        {
            break;
        }
    }

    return std::stoi(number);
    
}

/// @brief Initilization of String Arguments with an input string. Split up the input, assign the first item as the command, the rest as arguments
StringArguments::StringArguments(std::string input)
{
    std::vector<std::string> argument_vector = StringHelpers::splitString(input, ' ', false);
    arguments_string = std::accumulate(argument_vector.begin(), argument_vector.end(), std::string(""));
    if (argument_vector.size() > 0)
    {
        command = argument_vector[0];
        argument_vector.erase(argument_vector.begin());
        this->arguments = argument_vector;
    }
}

/// @brief returns the complete fen string which starts at argumentStartIndex
 std::string StringArguments::isolateFenString(int argument_start_index)
 {
    std::string fen = "";
    for (size_t i = argument_start_index; i < arguments.size(); i++)
    {
        if (arguments[i] == "moves")
            break;
        fen += arguments[i] + " ";
    }
    return fen;
 }

/// @brief returns the argument integer for a specific argument name. Ex command "go depth 1", "depth" will return 1.
/// Returns errorValue if argument is not found
int StringArguments::getNamedArgument(std::string argument_name, int error_value)
 {
    for (size_t i = 0; i < arguments.size(); i++)
    {
        if (arguments[i] == argument_name &&  i+1 < arguments.size()) //If the argument matches, return the next argument (the value)
            return std::stoi(arguments[i+1]);
    }
    return error_value;
 }