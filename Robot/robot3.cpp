#include "Robot.h"

#include <fstream>
#include <iostream>

int main()
{
	std::ifstream file("input.txt");
	std::string line;
	std::getline(file, line);
	file.close();

	Robot parser;
	const bool ok = parser.Parse(line);

	if (ok)
	{
		std::cout << "OK\n";
		std::cout << parser.GetTokenCount() << "\n";
	}
	else
	{
		std::cout << "ERROR\n";
		std::cout << parser.GetErrorPosition() << "\n";
	}

	return 0;
}