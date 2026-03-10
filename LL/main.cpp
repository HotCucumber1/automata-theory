#include "LLTableBuilder.h"
#include <iostream>

Grammar GetTask1Grammar();
Grammar GetWrongGrammar();
Grammar GetExampleFromPres();

int main()
{
	try
	{
		const auto g = GetTask1Grammar();

		g.Print();
		std::cout << std::endl;

		LLTableBuilder builder;
		builder.BuildTable(g);
		builder.PrintTable();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}

Grammar GetExampleFromPres()
{
	Grammar g;

	g.AddRule('S', "AB", true);

	g.AddRule('A', "a");
	g.AddRule('A', "cA");

	g.AddRule('B', "bA");

	return g;
}

Grammar GetTask1Grammar()
{
	Grammar g;
	g.AddRule('S', "E", true);

	g.AddRule('E', "TB");

	g.AddRule('B', "+TB");
	g.AddRule('B', "-TB");
	g.AddRule('B', "#");

	g.AddRule('T', "FC");

	g.AddRule('C', "*FC");
	g.AddRule('C', "/FC");
	g.AddRule('C', "#");

	g.AddRule('F', "IM");
	g.AddRule('F', "(E)");

	g.AddRule('M', "^N");
	g.AddRule('M', "#");

	g.AddRule('I', "a");
	g.AddRule('I', "b");
	g.AddRule('I', "c");
	g.AddRule('I', "d");

	g.AddRule('N', "2");
	g.AddRule('N', "3");
	g.AddRule('N', "4");

	return g;
}

Grammar GetWrongGrammar()
{
	Grammar g;

	g.AddRule('S', "Ac", true);
	g.AddRule('S', "dBa");

	g.AddRule('A', "Aa");
	g.AddRule('A', "Ab");
	g.AddRule('A', "faBf");

	g.AddRule('B', "cB");
	g.AddRule('B', "#");

	return g;
}