#include "../Model/MealyMachine.h"
#include "../Model/MooreMachine.h"
#include <iostream>

int main()
{
	try
	{
		MealyMachine mealy("S0");
		mealy.FromDot("./input/MealyToMin.dot");
		const auto newOne = mealy.GetMinimized();

		newOne->SaveToDot("./MealyToMinReady.dot");
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}