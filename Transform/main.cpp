#include "../Model/MealyMachine.h"
#include "../Model/MooreMachine.h"
#include <iostream>

int main()
{
	try
	{
		MealyMachine mealy("S0");
		mealy.FromDot("./input/mealy.dot");

		MooreMachine moore(mealy);
		MealyMachine newMealy(moore);

		newMealy.SaveToDot("./output_new_mealy.dot");
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}
