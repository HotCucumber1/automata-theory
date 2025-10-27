#include "../Model/MooreMachine.h"
#include <iostream>

int main()
{
	try
	{
		MooreMachine moore("A");
		moore.FromDot("./input/moore_max_2.dot");

		const auto newMoore = moore.GetMinimized();
		newMoore->SaveToDot("./moore_min_2.dot");
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}
