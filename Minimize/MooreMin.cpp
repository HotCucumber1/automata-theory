#include "../Model/MooreMachine.h"
#include <iostream>

int main()
{
	try
	{
		MooreMachine moore("S0");
		moore.FromDot("./input/moored_dfa_big.dot");

		const auto newMoore = moore.GetMinimized();
		newMoore->SaveToDot("./moored_dfa_big_res.dot");
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}
