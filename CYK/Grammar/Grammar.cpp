#include "Grammar.h"

#include <ranges>

Grammar::RulesMap Grammar::GetRules() const
{
	return m_rules;
}

void Grammar::AddRule(const char left, const std::string& right, const bool isAxiom)
{
	m_rules[left].push_back(right);
	if (isAxiom)
	{
		m_axiom = left;
	}
}

bool Grammar::IsNonTerminal(const char c)
{
	return isupper(c);
}

std::set<char> Grammar::GetNonTerminals() const
{
	std::set<char> nonTerminals;
	for (const auto& key : m_rules | std::views::keys)
	{
		nonTerminals.insert(key);
	}
	return nonTerminals;
}

char Grammar::GetAxiom() const
{
	return m_axiom;
}
