#include "../Model/MealyMachine.h"
#include "../Model/MooreMachine.h"
#include <iostream>

int main()
{
	try
	{
		MealyMachine mealy("S1");
		mealy.FromDot("./input/mealy.dot");

		MooreMachine moore(mealy);
		moore.SaveToDot("./output_moore.dot");

		// TODO убрать висячие вершины
		// TODO пустить волны

		MealyMachine newMealy(moore);
		newMealy.SaveToDot("./output_mealy.dot");
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}
