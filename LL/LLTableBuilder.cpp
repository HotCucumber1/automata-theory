#include "LLTableBuilder.h"

#include <iomanip>
#include <ios>
#include <iostream>

constexpr char EPSILON = '#';
constexpr char END_OF_FILE = '$';

void LLTableBuilder::BuildTable(const Grammar& g)
{
	BuildFirstAndFollow(g);
	ValidateLL1(g);

	table.clear();
	table.push_back({});

	size_t currentIndex = 1;
	std::map<char, int> lhsIndices;

	// Резервируем строки для левых частей (диспетчеров)
	for (const auto nt : g.GetNonTerminals())
	{
		lhsIndices[nt] = currentIndex;
		const auto& rules = g.GetRules().at(nt);
		currentIndex += rules.size();
	}

	// Заполняем строки левых частей
	for (char nt : g.GetNonTerminals())
	{
		const auto& rules = g.GetRules().at(nt);
		for (size_t i = 0; i < rules.size(); ++i)
		{
			TableRow row;
			row.id = lhsIndices[nt] + i;
			row.symbol = std::string(1, nt);
			row.guideSet = GetGuideSet(nt, rules[i]);
			row.stack = 0;
			row.error = (i == rules.size() - 1);
			row.end = false;
			row.shift = false;
			row.next = -1;
			table.push_back(row);
		}
	}

	// Проход 2: Строим строки для правых частей правил
	std::map<std::pair<char, int>, int> rhsStartIndices;

	for (char nt : g.GetNonTerminals())
	{
		const auto& rules = g.GetRules().at(nt);
		for (size_t i = 0; i < rules.size(); ++i)
		{
			rhsStartIndices[{ nt, i }] = currentIndex;
			const auto rule = rules[i];

			if (rule.empty())
			{
				TableRow row;
				row.id = currentIndex++;
				row.symbol = EPSILON;
				row.guideSet = GetGuideSet(nt, rule);
				row.next = 0;
				row.stack = 0;
				row.error = true;
				row.end = (nt == g.GetAxiom());
				row.shift = false;
				table.push_back(row);
			}
			else
			{
				for (size_t j = 0; j < rule.size(); ++j)
				{
					char X = rule[j];
					bool isLast = (j == rule.size() - 1);
					TableRow row;
					row.id = currentIndex++;
					row.symbol = std::string(1, X);

					if (!Grammar::IsNonTerminal(X))
					{
						row.guideSet = { X };
						row.next = isLast ? 0 : currentIndex;
						row.stack = 0;
						row.error = true;
						row.end = (isLast && nt == g.GetAxiom());
						row.shift = true;
					}
					else
					{
						row.guideSet = GetGuideForNT(X);
						row.next = lhsIndices[X];
						row.stack = isLast ? 0 : currentIndex;
						row.error = true;
						row.end = (isLast && nt == g.GetAxiom());
						row.shift = false;
					}
					table.push_back(row);
				}
			}
		}
	}

	// Обновляем указатели Next для строк левых частей
	for (char nt : g.GetNonTerminals())
	{
		const auto& rules = g.GetRules().at(nt);
		for (size_t i = 0; i < rules.size(); ++i)
		{
			auto rowId = lhsIndices[nt] + i;
			table[rowId].next = rhsStartIndices[{ nt, i }];
		}
	}
}

void LLTableBuilder::PrintTable() const
{
	std::cout << std::left
			  << std::setw(5) << "ID"
			  << std::setw(10) << "Symbol"
			  << std::setw(25) << "Guide Set"
			  << std::setw(10) << "Next"
			  << std::setw(10) << "Stack"
			  << std::setw(10) << "Error"
			  << std::setw(10) << "End"
			  << std::setw(10) << "Shift"
			  << std::endl;
	std::cout << std::string(90, '-') << std::endl;

	for (size_t i = 1; i < table.size(); ++i)
	{
		const auto& row = table[i];
		std::string guideStr;
		for (const auto c : row.guideSet)
		{
			guideStr += std::string(1, c) + " ";
		}

		std::cout << std::left
				  << std::setw(5) << row.id
				  << std::setw(10) << row.symbol
				  << std::setw(25) << guideStr
				  << std::setw(10) << row.next
				  << std::setw(10) << row.stack
				  << std::setw(10) << (row.error ? "+" : "-")
				  << std::setw(10) << (row.end ? "+" : "-")
				  << std::setw(10) << (row.shift ? "+" : "-")
				  << std::endl;
	}
}

std::set<char> LLTableBuilder::GetFirstOfSequence(const std::string& seq)
{
	std::set<char> res;
	if (seq.empty())
	{
		res.insert(EPSILON);
		return res;
	}
	auto allEps = true;
	for (const auto c : seq)
	{
		if (!Grammar::IsNonTerminal(c))
		{
			res.insert(c);
			allEps = false;
			break;
		}

		auto hasEps = false;
		for (char f : first[c])
		{
			if (f == EPSILON)
			{
				hasEps = true;
			}
			else
			{
				res.insert(f);
			}
		}
		if (!hasEps)
		{
			allEps = false;
			break;
		}
	}
	if (allEps)
	{
		res.insert(EPSILON);
	}
	return res;
}

std::set<char> LLTableBuilder::GetGuideSet(const char lhs, const std::string& rhs)
{
	std::set<char> guide = GetFirstOfSequence(rhs);
	if (guide.contains(EPSILON))
	{
		guide.erase(EPSILON);
		for (const auto f : follow[lhs])
		{
			guide.insert(f);
		}
	}
	return guide;
}

std::set<char> LLTableBuilder::GetGuideForNT(char X)
{
	std::set<char> res = first[X];
	if (res.contains(EPSILON))
	{
		res.erase(EPSILON);
		for (const auto f : follow[X])
		{
			res.insert(f);
		}
	}
	return res;
}

void LLTableBuilder::BuildFirstAndFollow(const Grammar& g)
{
	for (const auto nt : g.GetNonTerminals())
	{
		first[nt] = {};
		follow[nt] = {};
	}
	follow[g.GetAxiom()].insert(END_OF_FILE);

	// FISRT
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (auto const& pair : g.GetRules())
		{
			auto nt = pair.first;
			for (const auto& rule : pair.second)
			{
				std::set<char> seqFirst = GetFirstOfSequence(rule);
				for (const auto c : seqFirst)
				{
					if (first[nt].insert(c).second)
					{
						changed = true;
					}
				}
			}
		}
	}

	// FOLLOW
	changed = true;
	while (changed)
	{
		changed = false;
		for (auto const& pair : g.GetRules())
		{
			auto nt = pair.first;
			for (auto const& rule : pair.second)
			{
				for (size_t i = 0; i < rule.size(); ++i)
				{
					char B = rule[i];
					if (Grammar::IsNonTerminal(B))
					{
						std::string beta = rule.substr(i + 1);
						std::set<char> firstBeta = GetFirstOfSequence(beta);
						bool betaHasEps = false;
						for (const auto f : firstBeta)
						{
							if (f == EPSILON)
							{
								betaHasEps = true;
							}
							else if (follow[B].insert(f).second)
							{
								changed = true;
							}
						}
						if (betaHasEps || beta.empty())
						{
							for (const auto f : follow[nt])
							{
								if (follow[B].insert(f).second)
								{
									changed = true;
								}
							}
						}
					}
				}
			}
		}
	}
}

void LLTableBuilder::ValidateLL1(const Grammar& g)
{
	const auto orderedNonTerminals = g.GetNonTerminals();
	const auto& rulesMap = g.GetRules();

	for (const auto nt : orderedNonTerminals)
	{
		const auto& rules = rulesMap.at(nt);

		if (rules.size() <= 1)
		{
			continue;
		}

		std::set<char> seenTerminals;
		for (size_t i = 0; i < rules.size(); ++i)
		{
			std::set<char> guide = GetGuideSet(nt, rules[i]);
			for (const char c : guide)
			{
				if (seenTerminals.contains(c))
				{
					std::string errorMsg = "Грамматика не является LL(1)! ";
					errorMsg += "Найдено пересечение (конфликт) для нетерминала '";
					errorMsg += std::string(1, nt) + "' по символу '" + std::string(1, c) + "'.\n";
					errorMsg += "Альтернатива " + std::to_string(i + 1) + ": -> " + (rules[i].empty() ? "e" : rules[i]);

					throw std::runtime_error(errorMsg);
				}
				seenTerminals.insert(c);
			}
		}
	}
}