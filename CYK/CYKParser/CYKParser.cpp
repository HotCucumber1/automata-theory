#include "CYKParser.h"

#include <utility>

CYKParser::CYKParser(Grammar grammar)
	: m_grammar(std::move(grammar))
{
}

bool CYKParser::DoesWordBelongToGrammar(const std::string& word)
{
	const auto wordLength = word.length();
	if (word.empty())
	{
		return false;
	}

	Table table(wordLength, std::vector<std::set<char>>(wordLength));

	for (int i = 0; i < wordLength; i++)
	{
		const auto terminal = word[i];
		const auto nonTerminals = FindParentsForTerminal(terminal);
		table[i][i] = nonTerminals;
	}

	for (int length = 2; length <= wordLength; length++)
	{
		for (int i = 0; i <= wordLength - length; i++)
		{
			const int j = i + length - 1;
			std::set<char> nonTerminals;

			for (int k = i; k < j; k++)
			{
				const auto leftPart = table[i][k];
				const auto rightPart = table[k + 1][j];

				if (leftPart.empty() || rightPart.empty())
				{
					continue;
				}

				auto newNonTerminals = FindParentsForPair(i, k, j, table);
				nonTerminals.insert(newNonTerminals.begin(), newNonTerminals.end());
			}

			table[i][j] = nonTerminals;
		}
	}

	m_currentTable = table;
	m_currentWord = word;
	return m_currentTable[0][wordLength - 1].contains(m_grammar.GetAxiom());
}

void CYKParser::PrintCurrentTable(std::ostream& out) const
{
	const auto n = m_currentWord.length();

	out << "\nCYK Table for \"" << m_currentWord << "\":" << std::endl;
	size_t maxWidth = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = i; j < n; j++)
		{
			maxWidth = std::max(maxWidth, m_currentTable[i][j].size());
		}
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = i; j < n; j++)
		{
			out << "{";

			for (const auto& c : m_currentTable[i][j])
			{
				out << c;
			}

			out << "} ";
			const auto padding = maxWidth - m_currentTable[i][j].size();
			for (size_t p = 0; p < padding; p++)
			{
				out << " ";
			}
		}
		out << std::endl;
	}
}

std::set<char> CYKParser::FindParentsForTerminal(const char terminal) const
{
	std::set<char> result;
	for (const auto& rule : m_grammar.GetRules())
	{
		char lhs = rule.first;
		for (const std::string& rhs : rule.second)
		{
			if (rhs.length() == 1 && rhs[0] == terminal)
			{
				result.insert(lhs);
			}
		}
	}
	return result;
}

std::set<char> CYKParser::FindParentsForPair(
	const int i,
	const int k,
	const int j,
	const Table& table) const
{
	std::set<char> result;

	for (const auto& rule : m_grammar.GetRules())
	{
		char lhs = rule.first;
		for (const std::string& rhs : rule.second)
		{
			if (rhs.length() == 2)
			{
				const auto B = rhs[0];
				const auto C = rhs[1];

				if (table[i][k].contains(B) && table[k + 1][j].contains(C))
				{
					result.insert(lhs);
				}
			}
		}
	}

	return result;
}
