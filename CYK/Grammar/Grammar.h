#pragma once
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class Grammar
{
public:
	using RulesMap = std::unordered_map<char, std::vector<std::string>>;

	RulesMap GetRules() const;

	void AddRule(char left, const std::string& right, bool isAxiom = false);

	static bool IsNonTerminal(char c);

	std::set<char> GetNonTerminals() const;

	char GetAxiom() const;

private:
	RulesMap m_rules;
	char m_axiom = 'S';
};
