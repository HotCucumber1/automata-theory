#include "../Model/MooreMachine.h"

#include <iostream>

int main()
{
	try
	{
		MooreMachine moore("S0");
		moore.FromDot("./input/moore_big.dot");

		auto det = moore.GetDeterministic();
		det->SaveToDot("./moored_dfa_big.dot");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}