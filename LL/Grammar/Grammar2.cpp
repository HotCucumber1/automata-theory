#include "Grammar2.h"

const Grammar2::RulesMap& Grammar2::GetRules() const
{
	return m_rules;
}

void Grammar2::SetRules(const RulesMap& rules)
{
	m_rules = rules;
}

void Grammar2::AddRule(const RuleItem& left, const std::vector<RuleItem>& right, const bool isAxiom)
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

bool Grammar2::IsNonTerminal(const RuleItem& rule)
{
	return !rule.isTerminal;
}

std::vector<Grammar2::RuleItem> Grammar2::GetNonTerminals() const
{
	return m_orderedNonTerminals;
}

Grammar2::RuleItem Grammar2::GetAxiom() const
{
	return m_axiom;
}

void Grammar2::Print(std::ostream& out) const
{
	for (const auto& [left, right] : m_rules)
	{
		for (const auto& rhs : right)
		{
			for (const auto& [ruleName, isTerminal] : rhs)
			{
				out << left.ruleName << " -> " << ruleName << std::endl;
			}
		}
	}
}
