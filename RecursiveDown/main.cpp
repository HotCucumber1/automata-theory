#include "../Lexer/Lexer.h"
#include "SyntaxAnalyzer.h"

#include <fstream>

int main()
{
	try
	{
		std::ifstream file("input/new_lang");
		Lexer lexer(file);
		SyntaxAnalyzer syntaxAnalyzer(lexer);

		if (syntaxAnalyzer.Parse())
		{
			std::cout << "Parsed successfully" << std::endl;
		}
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
}