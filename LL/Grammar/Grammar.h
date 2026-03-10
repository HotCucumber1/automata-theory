#pragma once
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

class Grammar
{
public:
	using RulesMap = std::map<char, std::vector<std::string>>;

	const RulesMap& GetRules() const;

	void SetRules(const RulesMap& rules);

	void AddRule(char left, const std::string& right, bool isAxiom = false);

	static bool IsNonTerminal(char c);

	std::vector<char> GetNonTerminals() const;

	char GetAxiom() const;

	void Print(std::ostream& out = std::cout) const;

private:
	RulesMap m_rules;
	std::vector<char> m_orderedNonTerminals;
	char m_axiom = 'S';
};
