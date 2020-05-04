// Magnificence Chess.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "Board/BitBoard.h"

#include "Interface/Interface.h"

int main()
{
	Interface interface = Interface();
    interface.run();
    return 0;
}
