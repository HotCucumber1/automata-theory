#include "CNFConverter/CNFConverter.h"
#include "CYKParser/CYKParser.h"
#include "Grammar/Grammar.h"
#include <iostream>

Grammar GetSimpleGrammar1();
void CYK1();
void SimlpifiedGrammar1();

void CYK_Work1();
void CYK_Work2();
void CYK_Work3();
void CYK_Work4();
void CYK_Work5();
void CYK_Work6();
void CYK_Work7();

void Prividenie_Work1();
void Prividenie_Work2();
void Prividenie_Work3();


int main() // ε - epsilon
{
	try
	{
		Prividenie_Work2();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
}

void Prividenie_Work2()
{
	Grammar grammar;

	grammar.AddRule('S', "a", true);
	grammar.AddRule('S', "A");
	grammar.AddRule('S', "B");

	grammar.AddRule('A', "aB");
	grammar.AddRule('A', "bS");

	grammar.AddRule('B', "AB");
	grammar.AddRule('B', "Ba");
	grammar.AddRule('B', "Ad");
	grammar.AddRule('B', "Bb");

	grammar.AddRule('C', "Ad");
	grammar.AddRule('C', "Bb");
	grammar.AddRule('C', "AB");
	grammar.AddRule('C', "Ba");
	grammar.AddRule('C', "a");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);
}

void Prividenie_Work3()
{
	Grammar grammar;

	grammar.AddRule('S', "a", true);
	grammar.AddRule('S', "AB");
	grammar.AddRule('S', "AS");

	grammar.AddRule('A', "AB");

	grammar.AddRule('B', "b");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);
}

void Prividenie_Work1()
{
	Grammar grammar;

	grammar.AddRule('Z', "E+T", true);

	grammar.AddRule('E', "E");
	grammar.AddRule('E', "S+F");
	grammar.AddRule('E', "T");

	grammar.AddRule('F', "F");
	grammar.AddRule('F', "F*P");
	grammar.AddRule('F', "P");

	grammar.AddRule('P', "G");

	grammar.AddRule('T', "T+i");
	grammar.AddRule('T', "T*F");
	grammar.AddRule('T', "T-G");
	grammar.AddRule('T', "i");

	grammar.AddRule('G', "G");
	grammar.AddRule('G', "GG");
	grammar.AddRule('G', "F");

	grammar.AddRule('Q', "E");
	grammar.AddRule('Q', "E+F");
	grammar.AddRule('Q', "T");
	grammar.AddRule('Q', "S");

	grammar.AddRule('S', "a");
	grammar.AddRule('S', "d");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);
}

void CYK_Work7()
{
	const auto word = "bbbcaaa"; // bbbcaaa
	Grammar grammar;

	grammar.AddRule('P', "AB", true);
	grammar.AddRule('P', "BG");

	grammar.AddRule('A', "aA");
	grammar.AddRule('A', "ε");

	grammar.AddRule('B', "c");
	grammar.AddRule('B', "bB");

	grammar.AddRule('G', "c");
	grammar.AddRule('G', "bA");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);

	CYKParser parser(newGrammar);
	std::cout << "Does '" << word << "' belongs to grammar: "
			  << parser.DoesWordBelongToGrammar(word)
			  << std::endl;
	parser.PrintCurrentTable();
}

void CYK_Work6()
{
	const auto word = "bbbcaaa"; // bbbcaaa
	Grammar grammar;

	grammar.AddRule('P', "AB", true);
	grammar.AddRule('P', "BG");

	grammar.AddRule('A', "aA");
	grammar.AddRule('A', "ε");

	grammar.AddRule('B', "c");
	grammar.AddRule('B', "bB");

	grammar.AddRule('G', "c");
	grammar.AddRule('G', "bA");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);

	CYKParser parser(newGrammar);
	std::cout << "Does '" << word << "' belongs to grammar: "
			  << parser.DoesWordBelongToGrammar(word)
			  << std::endl;
	parser.PrintCurrentTable();
}

void CYK_Work5()
{
	const auto word = "fi=iti=i"; // fiti=i+i
	Grammar grammar;

	grammar.AddRule('S', "i=E", true);
	grammar.AddRule('S', "fEtS");
	grammar.AddRule('E', "E+E");
	grammar.AddRule('E', "i");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);

	CYKParser parser(newGrammar);
	std::cout << "Does '" << word << "' belongs to grammar: "
			  << parser.DoesWordBelongToGrammar(word)
			  << std::endl;
	parser.PrintCurrentTable();
}

void CYK_Work4()
{
	const auto word = "i=i-(i*i)";
	Grammar grammar;

	grammar.AddRule('S', "S;i=E", true);
	grammar.AddRule('S', "i=E");
	grammar.AddRule('E', "E*i");
	grammar.AddRule('E', "E+i");
	grammar.AddRule('E', "i-(E)");
	grammar.AddRule('E', "i");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);

	CYKParser parser(newGrammar);
	std::cout << "Does '" << word << "' belongs to grammar: "
			  << parser.DoesWordBelongToGrammar(word)
			  << std::endl;
	parser.PrintCurrentTable();
}

void CYK_Work3()
{
	const auto word = ",i"; // i,i,i   ii    ,i
	Grammar grammar;

	grammar.AddRule('S', "iA", true);
	grammar.AddRule('A', ",S");
	grammar.AddRule('A', "ε");
	// grammar.AddRule('A', "B");
	// grammar.AddRule('B', "ε");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);

	CYKParser parser(newGrammar);
	std::cout << "Does '" << word << "' belongs to grammar: "
			  << parser.DoesWordBelongToGrammar(word)
			  << std::endl;
	parser.PrintCurrentTable();
}

void CYK_Work2()
{
	const auto word = ""; // aaaaaa aa
	Grammar grammar;

	grammar.AddRule('S', "AAA", true);
	grammar.AddRule('S', "B");
	grammar.AddRule('A', "aA");
	grammar.AddRule('A', "B");
	grammar.AddRule('B', "ε");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);

	CYKParser parser(newGrammar);
	std::cout << "Does '" << word << "' belongs to grammar: "
			  << parser.DoesWordBelongToGrammar(word)
			  << std::endl;
	parser.PrintCurrentTable();
}

void CYK_Work1()
{
	const auto word = "abab"; // aaaa
	Grammar grammar;

	grammar.AddRule('S', "AB", true);
	grammar.AddRule('A', "AAa");
	grammar.AddRule('A', "ε");
	grammar.AddRule('B', "bBB");
	grammar.AddRule('B', "ε");

	CNFConverter converter(grammar);
	const auto newGrammar = converter.Convert();
	newGrammar.Print(std::cout);

	CYKParser parser(newGrammar);
	std::cout << "Does '" << word << "' belongs to grammar: "
			  << parser.DoesWordBelongToGrammar(word)
			  << std::endl;
	parser.PrintCurrentTable();
}

void SimlpifiedGrammar1()
{
	Grammar grammar;

	grammar.AddRule('S', "AAA", true);
	grammar.AddRule('S', "B");

	grammar.AddRule('A', "aA");
	grammar.AddRule('A', "B");
	grammar.AddRule('B', "ε");

	grammar.Print(std::cout);
	std::cout << "\n-----------------------\n";

	CNFConverter converter(grammar);
	auto newGrammar = converter.Convert();

	newGrammar.Print(std::cout);
}

void CYK1()
{
	const auto grammar1 = GetSimpleGrammar1(); // TODO не работает

	const std::string example = "abbaaa";

	CYKParser parser(grammar1);

	std::cout << "Does '" << example << "' belongs to grammar: " << parser.DoesWordBelongToGrammar(example) << std::endl;
	parser.PrintCurrentTable();
}

Grammar GetSimpleGrammar1()
{
	Grammar grammar;

	grammar.AddRule('S', "AS", true);
	grammar.AddRule('S', "BS");
	grammar.AddRule('S', "a");

	grammar.AddRule('A', "BB");
	grammar.AddRule('A', "a");

	grammar.AddRule('B', "AA");
	grammar.AddRule('B', "b");
	return grammar;
}