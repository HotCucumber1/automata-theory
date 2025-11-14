#include "../Model/MooreMachine.h"

#include <iostream>

int main()
{
	try
	{
		MooreMachine mooreMachine;
		mooreMachine.FromRegular("((a*(a|b)*a) | b* (c|b)* b | c* (c|a)* c)");

		mooreMachine.SaveToDot("./NFA_113.dot");

		const auto dfa = mooreMachine.GetDeterministic();
		// dfa->SaveToDot("./DFA_12.dot");

		const auto min = dfa->GetMinimized();
		min->SaveToDot("./MIN_113.dot");
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}