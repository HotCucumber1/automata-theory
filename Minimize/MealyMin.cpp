#include "../Model/MealyMachine.h"
#include "../Model/MooreMachine.h"
#include <iostream>

int main()
{
	try
	{
		MealyMachine mealy("A");
		mealy.FromDot("./input/mealy_max_2.dot");

		const auto newMealy = mealy.GetMinimized();
		newMealy->SaveToDot("./mealy_min_2.dot");
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}