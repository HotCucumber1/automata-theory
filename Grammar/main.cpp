#include "../Model/MooreMachine.h"
#include <iostream>

int main()
{
	try
	{
		MooreMachine machine;
		machine.FromGrammar("./input/left_grammar.gram");

		machine.SaveToDot("./grammar_left.dot");
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
}