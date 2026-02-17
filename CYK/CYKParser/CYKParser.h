#pragma once
#include "../Grammar/Grammar.h"

#include <iostream>

class CYKParser
{
public:
	using Table = std::vector<std::vector<std::set<char>>>;

	explicit CYKParser(Grammar grammar);

	bool DoesWordBelongToGrammar(const std::string& word);

	void PrintCurrentTable(std::ostream& out = std::cout) const;

private:
	std::set<char> FindParentsForTerminal(char terminal) const;

	std::set<char> FindParentsForPair(
		int i,
		int k,
		int j,
		const Table& table) const;

private:
	Grammar m_grammar;
	Table m_currentTable;
	std::string m_currentWord;
};
