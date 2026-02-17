#include "CYKParser/CYKParser.h"
#include "Grammar/Grammar.h"

#include <iostream>

int main()
{
	try
	{
		Grammar grammar;

		grammar.AddRule('S', "AS", true);
		grammar.AddRule('S', "BS");
		grammar.AddRule('S', "a");

		grammar.AddRule('A', "BB");
		grammar.AddRule('A', "a");

		grammar.AddRule('B', "AA");
		grammar.AddRule('B', "b");

		const std::string example = "abbaaa";

		CYKParser parser(grammar);

		std::cout << "Does '" << example << "' belongs to grammar: " <<  parser.DoesWordBelongToGrammar(example) << std::endl;
		parser.PrintCurrentTable();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
}