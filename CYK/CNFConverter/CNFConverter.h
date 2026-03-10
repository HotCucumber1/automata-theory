#pragma once
#include "../Grammar/Grammar.h"

class CNFConverter
{

public:
	explicit CNFConverter(Grammar  g);

	Grammar Convert();

private:
	char GetNewNonTerminal();

	void EliminateEpsilonRules();

	void EliminateUnitRules();

	void EliminateUselessSymbols();

	void ReplaceTerminals();

	void Binarize();

	Grammar m_grammar;
	char m_nextNonTerminal = 'A';
};
