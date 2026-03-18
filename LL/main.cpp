#include "LLTableBuilder.h"
#include "Parser.h"

#include <iostream>

Grammar GetTask1Grammar();
Grammar GetWrongGrammar();
Grammar GetExampleFromPres();
/**
 * Разобрали на паре
 */
Grammar GetClassExample();

Grammar ControlWork1();
Grammar ControlWork2();
Grammar ControlWork3();
Grammar ControlWork4();

int main()
{
	try
	{
		// const auto g = ControlWork3();
		//
		// g.Print();
		// std::cout << std::endl;
		//
		// LLTableBuilder builder; // TODO у правила # неверно определяется направляющее множество
		// builder.BuildTable(g);
		// builder.PrintTable();

		const auto input4 = "fb(a);a:=a+a*a;a:=be";
		const auto g4 = ControlWork4();

		const auto input1 = "w(a+b*(c+a))da(a+b,a)=a$";
		const auto g1 = ControlWork1();

		const auto input2 = "ta=i;b:ra:ie$";
		const auto g2 = ControlWork2();

		Parser parser(g2);
		if (parser.ParseInput(input2))
		{
			std::cout << "Parsed successfully" << std::endl;
			return 0;
		}
		std::cerr << "Error here:" << std::endl;
		parser.PrintCurrRow(std::cerr);
		std::cerr << "Parsed str: " << parser.GetParsedStr() << std::endl;
		std::cerr << "Current char: " << parser.GetCurrChar() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}

// F −> function I(I) S end
// S −> ; I:=E S | ε
// E −> E*I | E+I | I
// I -> a | b

Grammar ControlWork4()
{
	Grammar g;

	g.AddRule('F', "fI(I)Se", true);

	g.AddRule('S', ";I:=ES");
	g.AddRule('S', "#");

	g.AddRule('E', "IR");

	g.AddRule('R', "*IR");
	g.AddRule('R', "+IR");
	g.AddRule('R', "#");

	g.AddRule('I', "a");
	g.AddRule('I', "b");

	return g;
}

// Z → S ⊥
// S → P := E | if E then S | if E then S else S
// P → I | I (E)
// E → E+T |T
// T →Τ* F |F
// F → P | (E)
// I → a | b
Grammar ControlWork3()
{
	Grammar g;

	g.AddRule('Z', "S", true);

	g.AddRule('S', "P:=E");
	g.AddRule('S', "iEtSR");

	g.AddRule('R', "#");
	g.AddRule('R', "eS");

	g.AddRule('P', "IA");

	g.AddRule('A', "(E)");
	g.AddRule('A', "#");

	g.AddRule('E', "TB");

	g.AddRule('B', "+TB");
	g.AddRule('B', "#");

	g.AddRule('T', "FC");

	g.AddRule('C', "*FC");
	g.AddRule('C', "#");

	g.AddRule('F', "P");
	g.AddRule('F', "(E)");

	g.AddRule('I', "a");
	g.AddRule('I', "b");

	return g;
}

// S → type I = T B ⊥
// T -> int | record I: T B end
// B->e|;I:T B
// I -> a | b | c
Grammar ControlWork2()
{
	Grammar g;

	g.AddRule('S', "tI=TB$", true);

	g.AddRule('T', "i");
	g.AddRule('T', "rI:TBe");

	g.AddRule('B', "#");
	g.AddRule('B', ";I:TB");

	g.AddRule('I', "a");
	g.AddRule('I', "b");
	g.AddRule('I', "c");

	return g;
}

// S → P = E | while E do S
// P -> I | I (E A)
// A->e|,E A
// E -> E + T | T
// T -> T * F | F
// F -> P | (E)
// I -> a | b | c

Grammar ControlWork1()
{
	Grammar g;

	g.AddRule('S', "P=E", true);
	g.AddRule('S', "wEdS");

	g.AddRule('P', "IR");

	g.AddRule('R', "(EA)");
	g.AddRule('R', "#");

	g.AddRule('A', "#");
	g.AddRule('A', ",EA");

	g.AddRule('E', "TB");

	g.AddRule('B', "+TB");
	g.AddRule('B', "#");

	g.AddRule('T', "FC");

	g.AddRule('C', "*FC");
	g.AddRule('C', "#");

	g.AddRule('F', "P");
	g.AddRule('F', "(E)");

	g.AddRule('I', "a");
	g.AddRule('I', "b");
	g.AddRule('I', "c");

	return g;
}

Grammar GetClassExample()
{
	Grammar g;

	g.AddRule('S', "aAb$", true);

	g.AddRule('A', "cBd");

	g.AddRule('B', "#");
	g.AddRule('B', "fBg");
	g.AddRule('B', "k");

	return g;
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