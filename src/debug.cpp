#include "debug.h"
#include <iostream>
#include <string>

unsigned short DEBUG::get_break_point()
{
	std::cout << "Enter program break point (inclusive): ";
	unsigned short bp;
	std::cin >> bp;
	return bp;
}

char DEBUG::debug_menu()
{
	const std::string prompt =
		std::string("memory range:\t\tm\n") +
		"register contents:\tr\n" +
		"continue:\t\tc\n" +
		"continue until:\t\tu\n" +
		"quit:\t\t\tq\n\n";
	std::cout << prompt; 
	std::string choice;
	char c;
	do {
		std::cout << "choice: ";
		getline(std::cin, choice);
		c = tolower(choice[0]);
	} while (choice.size() >  1 || (c != 'm' && c != 'r' && c != 'c' &&
									c != 'u' && c != 'q'));
	return c;
}

