#pragma once
#include "./Grammar/Grammar.h"
#include "LLTableBuilder.h"

#include <stack>

class Parser
{
public:
	explicit Parser(const Grammar& grammar);

	bool ParseInput(const std::string& input);

	void PrintCurrRow(std::ostream& out = std::cout) const;

	std::string GetParsedStr() const;

	char GetCurrChar() const;

private:
	size_t m_rowIndex = 1;
	std::string m_parsedData;
	char m_currChar{};
	LLTableBuilder m_table;
	std::stack<size_t> m_stack;
};
