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

std::string StringHelpers::getDisplayString(BitBoard& board)
{
    std::string boardString = "\n";
	int c = 63;
	for (size_t y = 0; y < 8; y++)
	{
		for (size_t x = 0; x < 8; x++)
		{
			boardString += pieceToChar(board.mailboard().pieces[c]);
			c--;
		}
		boardString += "\n";
	}
	boardString += "\n";
	return boardString;
}


char StringHelpers::pieceToChar(int piece)
{
    switch (piece)
	{
	case 5: //White pieces
		return 'P';
	case 3:
		return 'R';
	case 4:
		return 'N';
	case 2:
		return 'B';
	case 1:
		return 'Q';
	case 0:
		return 'K';
	case 12: //Black pieces
		return 'p';
	case 10:
		return 'r';
	case 11:
		return 'n';
	case 9:
		return 'b';
	case 8:
		return 'q';
	case 7:
		return 'k';
	case 14: //Space
		return '.';
	}
	return 0;
}