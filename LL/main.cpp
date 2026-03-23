#include "Parser.h"
#include  "Grammar/Grammar.h"

#include <iostream>

Grammar GetTask1Grammar();
Grammar GetWrongGrammar();
Grammar GetExampleFromPres();
Grammar GetClassExample();

Grammar ControlWork1();
Grammar ControlWork2();
Grammar ControlWork3();
Grammar ControlWork4();

Grammar GetConditionGrammar();
Grammar2 GetConditionGrammar2();

int main()
{
	try
	{
		// const auto g = ControlWork3();
		//
		// g.Print();
		// std::cout << std::endl;
		//
		// LLTableBuilder builder;
		// builder.BuildTable(g);
		// builder.PrintTable();

		const auto input4 = "fb(a);a:=a+a*a;a:=be";
		const auto g4 = ControlWork4();

		const auto input1 = "w(a+b*(c+a))da(a+b,a)=a$";
		const auto g1 = ControlWork1();

		const auto input2 = "ta=i;b:ra:ie$";
		const auto g2 = ControlWork2();

		const auto condInput1 = "true and false $";
		const auto condInput2 = "( q + w ) <= q * w and true and false or ( q > w + r ) $";
		const auto condGrammar = GetConditionGrammar2();

		Parser parser(condGrammar);
		if (parser.ParseInput(condInput2))
		{
			std::cout << "Parsed successfully" << std::endl;
			return 0;
		}
		std::cerr << "Error here:" << std::endl;
		parser.PrintCurrRow(std::cerr);
		std::cerr << "Parsed str: ";
		for (const auto& token : parser.GetParsedStr())
		{
			std::cerr << token.ruleName << ' ';
		}
		std::cerr << std::endl;
		std::cerr << "Current char: " << parser.GetCurrToken().ruleName << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}
/*
 * Con -> Con or Con1 | Con1
 * Con1 -> Con1 and Con2 | Con2
 * Con2 -> not Con2 | true | false | (Con) | RCon
 * RCon -> Expr ROp Expr | Expr
 * ROp -> < | > | <= | >= | == | <>
 * Expr -> Expr + Expr1 | Expr - Expr1 | Expr1
 * Expr1 -> Expr1 MultiOp Expr2 | Expr2
 * MultiOp -> * | / | mod | div
 * Expr2 -> -Expr2 | (Expr) | LargeIdent | NUM
 * LargeIdent -> Ident X
 * X -> # | .Ident X | [Expr] X
 */

// A    -> B C
// C    -> or B C | ε
// B    -> D E
// E    -> and D E | ε
// D    -> not D | true | false | (A) | F
// F    -> G H I | G
// H    -> < | > | <= | >= | == | <>
// G    -> J K
// K    -> + J K | - J K | ε
// J    -> L M
// M    -> N L M | ε
// N    -> * | / | mod | div
// L    -> - L | (G) | O | NUM
// O    -> P Q
// Q    -> # | . P Q | [G] Q

Grammar2 GetConditionGrammar2()
{
	Grammar2 g;

	// Вспомогательные лямбды для краткости (если хочешь)
	auto NT = [](const std::string& name) { return RuleItem{ name, false }; };
	auto T = [](const std::string& name) { return RuleItem{ name, true }; };
	auto EPS = RuleItem{ "#", true };

	// --- Корень и Логические уровни (OR, AND) ---
	g.AddRule(NT("S"), { NT("A"), T("$") });
	g.AddRule(NT("A"), { NT("B"), NT("C") });
	g.AddRule(NT("C"), { T("or"), NT("B"), NT("C") });
	g.AddRule(NT("C"), { EPS });

	g.AddRule(NT("B"), { NT("D"), NT("E") });
	g.AddRule(NT("E"), { T("and"), NT("D"), NT("E") });
	g.AddRule(NT("E"), { EPS });

	// --- Логическое НЕ и Сравнение ---
	g.AddRule(NT("D"), { T("not"), NT("D") });
	g.AddRule(NT("D"), { NT("F") });
	g.AddRule(NT("F"), { NT("G"), NT("X") });
	g.AddRule(NT("X"), { NT("H"), NT("I") });
	g.AddRule(NT("X"), { EPS });
	g.AddRule(NT("I"), { NT("G") });

	// --- Операторы сравнения (факторизованные) ---
	g.AddRule(NT("H"), { T("<=") });
	g.AddRule(NT("H"), { T(">=") });
	g.AddRule(NT("H"), { T("<>") });
	g.AddRule(NT("H"), { T("==") });
	g.AddRule(NT("H"), { T("<") });
	g.AddRule(NT("H"), { T(">") });

	// --- Арифметика (Сложение/Вычитание) ---
	g.AddRule(NT("G"), { NT("J"), NT("K") });
	g.AddRule(NT("K"), { T("+"), NT("J"), NT("K") });
	g.AddRule(NT("K"), { T("-"), NT("J"), NT("K") });
	g.AddRule(NT("K"), { EPS });

	// --- Арифметика (Умножение/Деление) ---
	g.AddRule(NT("J"), { NT("L"), NT("M") });
	g.AddRule(NT("M"), { NT("N"), NT("L"), NT("M") });
	g.AddRule(NT("M"), { EPS });

	g.AddRule(NT("N"), { T("*") });
	g.AddRule(NT("N"), { T("/") });
	g.AddRule(NT("N"), { T("mod") });
	g.AddRule(NT("N"), { T("div") });

	// --- Атомарные выражения ---
	g.AddRule(NT("L"), { T("-"), NT("L") }); // Унарный минус
	g.AddRule(NT("L"), { T("("), NT("A"), T(")") });
	g.AddRule(NT("L"), { NT("O") }); // Переменные
	g.AddRule(NT("L"), { NT("Z") }); // Числа
	g.AddRule(NT("L"), { T("true") });
	g.AddRule(NT("L"), { T("false") });

	// --- Идентификаторы (Переменные) ---
	g.AddRule(NT("O"), { NT("P"), NT("Q") });
	g.AddRule(NT("Q"), { T("."), NT("P"), NT("Q") });
	g.AddRule(NT("Q"), { T("["), NT("G"), T("]"), NT("Q") });
	g.AddRule(NT("Q"), { EPS });

	g.AddRule(NT("P"), { T("q"), NT("R") });
	g.AddRule(NT("P"), { T("w"), NT("R") });
	g.AddRule(NT("P"), { T("r"), NT("R") });

	g.AddRule(NT("R"), { T("q"), NT("R") });
	g.AddRule(NT("R"), { T("w"), NT("R") });
	g.AddRule(NT("R"), { T("r"), NT("R") });
	g.AddRule(NT("R"), { T("0"), NT("R") });
	g.AddRule(NT("R"), { T("1"), NT("R") });
	g.AddRule(NT("R"), { EPS });

	for (int i = 0; i <= 9; ++i)
	{
		std::string digit = std::to_string(i);
		g.AddRule(NT("Z"), { T(digit), NT("Y") });
		g.AddRule(NT("Y"), { T(digit), NT("Y") });
	}
	g.AddRule(NT("Y"), { EPS });

	return g;
}

Grammar GetConditionGrammar()
{
	Grammar g;
	// --- Корень и Логические уровни (OR, AND) ---
	g.AddRule('S', "A$", true);
	g.AddRule('A', "BC");
	g.AddRule('C', "oBC"); // o - OR
	g.AddRule('C', "#");
	g.AddRule('B', "DE");
	g.AddRule('E', "aDE"); // a - AND
	g.AddRule('E', "#");

	// --- Логическое НЕ и Сравнение ---
	g.AddRule('D', "nD"); // n - NOT
	g.AddRule('D', "F");
	g.AddRule('F', "GX"); // Левая факторизация для сравнений
	g.AddRule('X', "HI"); // H - оператор, I - правая часть
	g.AddRule('X', "#");
	g.AddRule('I', "G"); // Правая часть сравнения - это арифм. выражение

	// --- Операторы сравнения (полная левая факторизация) ---
	g.AddRule('H', "<U");
	g.AddRule('U', "="); // <=
	g.AddRule('U', ">"); // <>
	g.AddRule('U', "#"); // <
	g.AddRule('H', ">V");
	g.AddRule('V', "="); // >=
	g.AddRule('V', "#"); // >
	g.AddRule('H', "=W");
	g.AddRule('W', "="); // ==

	// --- Арифметика (Сложение/Вычитание) ---
	g.AddRule('G', "JK");
	g.AddRule('K', "+JK");
	g.AddRule('K', "-JK");
	g.AddRule('K', "#");

	// --- Арифметика (Умножение/Деление) ---
	g.AddRule('J', "LM");
	g.AddRule('M', "NLM");
	g.AddRule('M', "#");
	g.AddRule('N', "*");
	g.AddRule('N', "/");
	g.AddRule('N', "m"); // m - MOD
	g.AddRule('N', "d"); // d - DIV

	// --- Атомарные выражения (Скобки, Переменные, Числа) ---
	g.AddRule('L', "-L"); // Унарный минус
	g.AddRule('L', "(A)"); // Единый вход для скобок (решает конфликт D и L)
	g.AddRule('L', "O"); // Переменные
	g.AddRule('L', "Z"); // Числа
	g.AddRule('L', "t"); // t - true
	g.AddRule('L', "f"); // f - false

	// --- Переменные и свойства (Идентификаторы) ---
	g.AddRule('O', "PQ");
	g.AddRule('Q', ".PQ");
	g.AddRule('Q', "[G]Q");
	g.AddRule('Q', "#");
	g.AddRule('P', "qR"); // Начинается с q, w или r
	g.AddRule('P', "wR");
	g.AddRule('P', "rR");
	g.AddRule('R', "qR"); // Рекурсия для имен любой длины
	g.AddRule('R', "wR");
	g.AddRule('R', "rR");
	g.AddRule('R', "0R");
	g.AddRule('R', "1R");
	// ... и так далее для всех букв/цифр в R ...
	g.AddRule('R', "#");

	// --- Числа (Многоразрядные) ---
	g.AddRule('Z', "0Y");
	g.AddRule('Z', "1Y");
	g.AddRule('Z', "2Y");
	g.AddRule('Z', "3Y");
	g.AddRule('Z', "4Y");
	g.AddRule('Z', "5Y");
	g.AddRule('Z', "6Y");
	g.AddRule('Z', "7Y");
	g.AddRule('Z', "8Y");
	g.AddRule('Z', "9Y");
	g.AddRule('Y', "0Y");
	g.AddRule('Y', "1Y");
	g.AddRule('Y', "2Y");
	g.AddRule('Y', "3Y");
	g.AddRule('Y', "4Y");
	g.AddRule('Y', "5Y");
	g.AddRule('Y', "6Y");
	g.AddRule('Y', "7Y");
	g.AddRule('Y', "8Y");
	g.AddRule('Y', "9Y");
	g.AddRule('Y', "#");

	return g;
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