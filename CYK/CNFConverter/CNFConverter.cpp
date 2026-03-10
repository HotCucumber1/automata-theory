#include "CNFConverter.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <utility>

CNFConverter::CNFConverter(Grammar g)
	: m_grammar(std::move(g))
{
}

char CNFConverter::GetNewNonTerminal()
{
	while (m_grammar.GetRules().contains(m_nextNonTerminal) || m_nextNonTerminal == 'S')
	{
		m_nextNonTerminal++;
		if (m_nextNonTerminal > 'Z')
		{
			m_nextNonTerminal = 'A';
		}
	}
	return m_nextNonTerminal++;
}

void CNFConverter::EliminateEpsilonRules()
{
	std::set<char> nullable;

	bool changed;
	do
	{
		changed = false;
		for (const auto& rule : m_grammar.GetRules())
		{
			char A = rule.first;
			if (nullable.contains(A))
			{
				continue;
			}

			for (const std::string& rhs : rule.second)
			{
				if (rhs == "ε" || rhs.empty())
				{
					nullable.insert(A);
					changed = true;
					break;
				}

				bool allNullable = true;
				for (char c : rhs)
				{
					if (!Grammar::IsNonTerminal(c) || !nullable.contains(c))
					{
						allNullable = false;
						break;
					}
				}
				if (allNullable)
				{
					nullable.insert(A);
					changed = true;
					break;
				}
			}
		}
	} while (changed);

	std::unordered_map<char, std::vector<std::string>> newRules;

	for (const auto& rule : m_grammar.GetRules())
	{
		char A = rule.first;
		for (const std::string& rhs : rule.second)
		{
			if (rhs == "ε" || rhs.empty())
				continue;

			std::vector<std::string> combinations;
			combinations.emplace_back("");

			for (char c : rhs)
			{
				std::vector<std::string> newCombinations;
				for (const std::string& comb : combinations)
				{
					newCombinations.push_back(comb + c);

					if (Grammar::IsNonTerminal(c) && nullable.contains(c))
					{
						newCombinations.push_back(comb);
					}
				}
				combinations = newCombinations;
			}

			for (const std::string& comb : combinations)
			{
				if (!comb.empty())
				{
					newRules[A].push_back(comb);
				}
			}
		}
	}

	for (const auto& rule : m_grammar.GetRules())
	{
		if (!newRules.contains(rule.first))
		{
			newRules[rule.first] = rule.second;
		}
	}

	for (auto& rule : newRules)
	{
		std::set uniqueRules(rule.second.begin(), rule.second.end());
		rule.second.assign(uniqueRules.begin(), uniqueRules.end());
	}

	m_grammar.SetRules(newRules);
}

void CNFConverter::EliminateUnitRules()
{
	std::map<char, std::set<char>> unitPairs;

	bool changed;
	do
	{
		changed = false;
		for (const auto& rule : m_grammar.GetRules())
		{
			char A = rule.first;
			for (const std::string& rhs : rule.second)
			{
				if (rhs.length() == 1 && Grammar::IsNonTerminal(rhs[0]))
				{
					char B = rhs[0];
					if (!unitPairs[A].contains(B))
					{
						unitPairs[A].insert(B);
						changed = true;
					}
				}
			}
		}
	} while (changed);

	std::unordered_map<char, std::vector<std::string>> newRules;

	for (const auto& rule : m_grammar.GetRules())
	{
		char A = rule.first;
		for (const std::string& rhs : rule.second)
		{
			if (!(rhs.length() == 1 && Grammar::IsNonTerminal(rhs[0])))
			{
				newRules[A].push_back(rhs);
			}
		}
	}

	for (const auto& pair : unitPairs)
	{
		char A = pair.first;
		for (char B : pair.second)
		{
			const auto rules = m_grammar.GetRules();
			auto rulesIt = rules.find(B);
			if (rulesIt != rules.end())
			{
				for (const std::string& rhs : rulesIt->second)
				{
					if (!(rhs.length() == 1 && Grammar::IsNonTerminal(rhs[0])))
					{
						newRules[A].push_back(rhs);
					}
				}
			}
		}
	}

	for (auto& rule : newRules)
	{
		std::set<std::string> uniqueRules(rule.second.begin(), rule.second.end());
		rule.second.assign(uniqueRules.begin(), uniqueRules.end());
	}

	m_grammar.SetRules(newRules);
}

void CNFConverter::EliminateUselessSymbols()
{
	std::set<char> generating;
	bool changed;

	do
	{
		changed = false;
		for (const auto& rule : m_grammar.GetRules())
		{
			char A = rule.first;
			if (generating.contains(A))
			{
				continue;
			}

			for (const std::string& rhs : rule.second)
			{
				bool allGenerating = true;
				for (char c : rhs)
				{
					if (Grammar::IsNonTerminal(c) && !generating.contains(c))
					{
						allGenerating = false;
						break;
					}
				}
				if (allGenerating)
				{
					generating.insert(A);
					changed = true;
					break;
				}
			}
		}
	} while (changed);

	std::unordered_map<char, std::vector<std::string>> afterGenerating;

	for (const auto& rule : m_grammar.GetRules())
	{
		char A = rule.first;
		if (!generating.contains(A))
		{
			continue;
		}

		for (const std::string& rhs : rule.second)
		{
			bool allRhsGenerating = true;
			for (char c : rhs)
			{
				if (Grammar::IsNonTerminal(c) && !generating.contains(c))
				{
					allRhsGenerating = false;
					break;
				}
			}

			if (allRhsGenerating)
			{
				afterGenerating[A].push_back(rhs);
			}
		}
	}

	m_grammar.SetRules(afterGenerating);

	std::set<char> reachable;
	reachable.insert(m_grammar.GetAxiom());

	do
	{
		changed = false;
		std::set<char> newReachable = reachable;

		for (char A : reachable)
		{
			const auto rules = m_grammar.GetRules();
			auto rulesIt = rules.find(A);
			if (rulesIt != rules.end())
			{
				for (const std::string& rhs : rulesIt->second)
				{
					for (char c : rhs)
					{
						if (Grammar::IsNonTerminal(c) && !newReachable.contains(c))
						{
							newReachable.insert(c);
							changed = true;
						}
					}
				}
			}
		}
		reachable = newReachable;
	} while (changed);

	std::unordered_map<char, std::vector<std::string>> finalRules;

	for (char A : reachable)
	{
		const auto rules = m_grammar.GetRules();
		auto rulesIt = rules.find(A);
		if (rulesIt != rules.end())
		{
			finalRules[A] = rulesIt->second;
		}
	}

	m_grammar.SetRules(finalRules);
}

void CNFConverter::ReplaceTerminals()
{
	std::unordered_map<char, char> terminalMap;
	std::unordered_map<char, std::vector<std::string>> newRules;

	for (const auto& rule : m_grammar.GetRules())
	{
		char A = rule.first;
		for (const std::string& rhs : rule.second)
		{
			if (rhs.length() == 1)
			{
				newRules[A].push_back(rhs);
			}
		}
	}

	for (const auto& rule : m_grammar.GetRules())
	{
		char A = rule.first;
		for (const std::string& rhs : rule.second)
		{
			if (rhs.length() <= 1)
				continue;

			std::string newRhs;
			for (char c : rhs)
			{
				if (Grammar::IsNonTerminal(c))
				{
					newRhs += c;
				}
				else
				{
					if (!terminalMap.contains(c))
					{
						char newNT = GetNewNonTerminal();
						terminalMap[c] = newNT;
						newRules[newNT].emplace_back(1, c);
					}
					newRhs += terminalMap[c];
				}
			}
			newRules[A].push_back(newRhs);
		}
	}

	m_grammar.SetRules(newRules);
}

void CNFConverter::Binarize()
{
	std::unordered_map<char, std::vector<std::string>> newRules;

	for (const auto& rule : m_grammar.GetRules())
	{
		char A = rule.first;
		for (const std::string& rhs : rule.second)
		{
			if (rhs.length() <= 2)
			{
				newRules[A].push_back(rhs);
			}
			else
			{
				std::vector<char> tempNonTerminals;
				char current = A;

				for (size_t i = 0; i < rhs.length() - 2; i++)
				{
					const char newNT = GetNewNonTerminal();
					std::string part;
					part += rhs[i];
					part += (i == 0) ? rhs[i + 1] : newNT;

					newRules[current].push_back(part);
					current = newNT;
				}

				std::string lastPart;
				lastPart += rhs[rhs.length() - 2];
				lastPart += rhs[rhs.length() - 1];
				newRules[current].push_back(lastPart);
			}
		}
	}

	m_grammar.SetRules(newRules);
}

Grammar CNFConverter::Convert()
{
	EliminateEpsilonRules();
	// EliminateUnitRules();
	// EliminateUselessSymbols();
	// ReplaceTerminals();
	// Binarize();
	EliminateUselessSymbols();

	return m_grammar;
}