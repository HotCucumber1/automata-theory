#include "SLRTableBuilder.h"

Grammar2 GetSimpleGrammar();
Grammar2 GetWithLeftRecursion();
Grammar2 GetJustAnotherGrammar();
Grammar2 GetGrammarWithEpsilon();

int main()
{
	try
	{
		const auto gr = GetJustAnotherGrammar();
		const SLRTableBuilder table(gr);

		table.Print();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}

Grammar2 GetSimpleGrammar()
{
	Grammar2 grammar;

	grammar.AddRule(NT("S"), { T("a"), NT("S"), T("b") }, true);
	grammar.AddRule(NT("S"), { T("c") }, true);

	return grammar;
}

Grammar2 GetWithLeftRecursion()
{
	Grammar2 grammar;

	grammar.AddRule(NT("S"), { NT("A"), NT("B") }, true);
	grammar.AddRule(NT("A"), { T("a") });
	grammar.AddRule(NT("A"), { NT("A"), T("c") });
	grammar.AddRule(NT("B"), { T("b"), NT("A") });

	return grammar;
}

Grammar2 GetJustAnotherGrammar()
{
	Grammar2 grammar;

	grammar.AddRule(NT("S"), { T("a"), NT("A"), T("b") }, true);
	grammar.AddRule(NT("S"), { T("c"), NT("B"), T("b") }, true);

	grammar.AddRule(NT("A"), { T("x"), NT("A"), T("y") });
	grammar.AddRule(NT("A"), { T("a") });

	grammar.AddRule(NT("B"), { T("y"), NT("B"), T("x") });
	grammar.AddRule(NT("B"), { T("b") });

	return grammar;
}

Grammar2 GetGrammarWithEpsilon()
{
	Grammar2 grammar;

	grammar.AddRule(NT("S"), { NT("A"), NT("B"), NT("C") }, true);

	grammar.AddRule(NT("A"), { NT("A"), T("a") });
	grammar.AddRule(NT("A"), { EPS });

	grammar.AddRule(NT("B"), { T("b"), NT("B") });
	grammar.AddRule(NT("B"), { EPS });

	grammar.AddRule(NT("C"), { NT("C"), T("c") });
	grammar.AddRule(NT("C"), { EPS });

	return grammar;
}