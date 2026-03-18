#include "Grammar.h"

#include <ostream>
#include <ranges>

const Grammar::RulesMap& Grammar::GetRules() const
{
	return m_rules;
}

void Grammar::SetRules(const RulesMap& rules)
{
	m_rules = rules;
}

void Grammar::AddRule(
	const char left,
	const std::string& right,
	const bool isAxiom)
{
	if (!m_rules.contains(left))
	{
		m_orderedNonTerminals.push_back(left);
	}

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

std::vector<char> Grammar::GetNonTerminals() const
{
	return m_orderedNonTerminals;
}

char Grammar::GetAxiom() const
{
	return m_axiom;
}

void Grammar::Print(std::ostream& out) const
{
	// for (const auto & m_rule : std::ranges::reverse_view(m_rules))
	// {
	// 	const auto& left = m_rule.first;
	// 	const auto& right = m_rule.second;
	//
	// 	for (const auto& rhs : right)
	// 	{
	// 		out << left << " -> " << rhs << std::endl;
	// 	}
	// }
	for (const auto& [left, right] : m_rules)
	{
		for (const auto& rhs : right)
		{
			out << left << " -> " << rhs << std::endl;
		}
	}
}
