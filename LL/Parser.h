#pragma once
#include "./Grammar/Grammar2.h"
#include "LLTableBuilder.h"

#include <stack>

class Parser
{
public:
	explicit Parser(const Grammar2& grammar);

	bool ParseInput(const std::string& inputStr);

	void PrintCurrRow(std::ostream& out = std::cout) const;

	std::vector<RuleItem> GetParsedStr() const;

	RuleItem GetCurrToken() const;

private:
	static std::vector<RuleItem> TokenizeString(const std::string& input);

private:
	size_t m_rowIndex = 1;
	std::vector<RuleItem> m_parsedTokens;
	RuleItem m_currToken;
	LLTableBuilder m_table;
	std::stack<size_t> m_stack;
};
