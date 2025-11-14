#include "../Model/MooreMachine.h"

#include <iostream>

int main()
{
	try
	{
		MooreMachine moore("S0");
		moore.FromDot("./input/from_lec.dot");

		const auto det = moore.GetDeterministic();
		const auto newMachine = det->GetMinimized();

		newMachine->SaveToDot("./min_from_lec_out.dot");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}