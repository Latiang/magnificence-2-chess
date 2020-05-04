 #include "StringHelpers.h"
 
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

    //std::vector<std::string> lowercase_output;
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