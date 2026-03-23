#include "LLTableBuilder.h"

#include <iomanip>
#include <ios>
#include <iostream>

void LLTableBuilder::BuildTable(const Grammar2& g)
{
	BuildFirstAndFollow(g);
	ValidateLL1(g);

	m_table.clear();
	m_table.push_back({});

	size_t currentIndex = 1;
	std::map<RuleItem, size_t> lhsIndices;

	// Резервируем строки для левых частей (диспетчеров)
	for (const auto nt : g.GetNonTerminals())
	{
		lhsIndices[nt] = currentIndex;
		const auto& rules = g.GetRules().at(nt);
		currentIndex += rules.size();
	}

	// Заполняем строки левых частей
	for (const auto& nonTerminal : g.GetNonTerminals())
	{
		const auto& rules = g.GetRules().at(nonTerminal);
		for (size_t i = 0; i < rules.size(); ++i)
		{
			TableRow row;
			row.id = lhsIndices[nonTerminal] + i;
			row.leftPart = nonTerminal.ruleName;
			row.guideSet = GetGuideSet(nonTerminal, rules[i]);
			row.stack = 0;
			row.error = (i == rules.size() - 1);
			row.end = false;
			row.shift = false;
			row.next = -1;
			m_table.push_back(row);
		}
	}

	// Строим строки для правых частей правил
	std::map<std::pair<RuleItem, int>, int> rhsStartIndices;

	for (const auto& nonTerminal : g.GetNonTerminals())
	{
		const auto& rules = g.GetRules().at(nonTerminal);
		for (size_t i = 0; i < rules.size(); ++i)
		{
			rhsStartIndices[{ nonTerminal, i }] = currentIndex;
			const auto rule = rules[i];

			if (rule.empty())
			{
				TableRow row;
				row.id = currentIndex++;
				row.leftPart = EPSILON;
				row.guideSet = GetGuideSet(nonTerminal, rule);
				row.next = 0;
				row.stack = 0;
				row.error = true;
				row.end = (nonTerminal == g.GetAxiom());
				row.shift = false;
				m_table.push_back(row);
			}
			else
			{
				for (size_t j = 0; j < rule.size(); ++j)
				{
					auto X = rule[j];
					bool isLast = (j == rule.size() - 1);
					TableRow row;
					row.id = currentIndex++;
					row.leftPart = X.ruleName;

					if (!Grammar2::IsNonTerminal(X))
					{
						row.guideSet = { X };
						row.next = isLast ? 0 : currentIndex;
						row.stack = 0;
						row.error = true;
						row.end = (isLast && nonTerminal == g.GetAxiom());
						row.shift = X.ruleName != EPSILON;
					}
					else
					{
						row.guideSet = GetGuideForNT(X);
						row.next = lhsIndices[X];
						row.stack = isLast ? 0 : currentIndex;
						row.error = true;
						row.end = (isLast && nonTerminal == g.GetAxiom());
						row.shift = false;
					}
					m_table.push_back(row);
				}
			}
		}
	}

	// Обновляем указатели Next для строк левых частей
	for (const auto& nt : g.GetNonTerminals())
	{
		const auto& rules = g.GetRules().at(nt);
		for (size_t i = 0; i < rules.size(); ++i)
		{
			auto rowId = lhsIndices[nt] + i;
			m_table[rowId].next = rhsStartIndices[{ nt, i }];
		}
	}

	TransformRowsToMap();
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

	for (size_t i = 1; i < m_table.size(); ++i)
	{
		const auto& row = m_table[i];
		std::string guideStr;
		for (const auto& c : row.guideSet)
		{
			guideStr += c.ruleName + " ";
		}

		std::cout << std::left
				  << std::setw(5) << row.id
				  << std::setw(10) << row.leftPart
				  << std::setw(25) << guideStr
				  << std::setw(10) << row.next
				  << std::setw(10) << row.stack
				  << std::setw(10) << (row.error ? "true" : "false")
				  << std::setw(10) << (row.end ? "true" : "false")
				  << std::setw(10) << (row.shift ? "true" : "false")
				  << std::endl;
	}
}
std::unordered_map<size_t, TableRow> LLTableBuilder::GetTable() const
{
	if (m_rows.empty())
	{
		throw std::runtime_error("Table is not initialised");
	}
	return m_rows;
}

std::set<RuleItem> LLTableBuilder::GetFirstOfSequence(const std::vector<RuleItem>& seq)
{
	std::set<RuleItem> res;
	if (seq.empty())
	{
		res.insert({ EPSILON, true });
		return res;
	}
	auto allEps = true;
	for (const auto c : seq)
	{
		if (!Grammar2::IsNonTerminal(c))
		{
			res.insert(c);
			allEps = false;
			break;
		}

		auto hasEps = false;
		for (const auto& f : m_first[c])
		{
			if (f.ruleName == EPSILON)
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
		res.insert({ EPSILON, true });
	}
	return res;
}

std::set<RuleItem> LLTableBuilder::GetGuideSet(const RuleItem& lhs, const std::vector<RuleItem>& rhs)
{
	auto guide = GetFirstOfSequence(rhs);
	if (guide.contains({ EPSILON, true }))
	{
		guide.erase({ EPSILON, true });
		for (const auto& f : m_follow[lhs])
		{
			guide.insert(f);
		}
	}
	return guide;
}

std::set<RuleItem> LLTableBuilder::GetGuideForNT(const RuleItem& X)
{
	auto res = m_first[X];
	if (res.contains({ EPSILON, true }))
	{
		res.erase({ EPSILON, true });
		for (const auto& f : m_follow[X])
		{
			res.insert(f);
		}
	}
	return res;
}

void LLTableBuilder::BuildFirstAndFollow(const Grammar2& g)
{
	for (const auto& nt : g.GetNonTerminals())
	{
		m_first[nt] = {};
		m_follow[nt] = {};
	}
	m_follow[g.GetAxiom()].insert({ END_OF_FILE, true });

	// FIRST
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (auto const& pair : g.GetRules())
		{
			auto nt = pair.first;
			for (const auto& rule : pair.second)
			{
				auto seqFirst = GetFirstOfSequence(rule);
				for (const auto& c : seqFirst)
				{
					if (m_first[nt].insert(c).second)
					{
						changed = true;
					}
				}
			}
		}
	}

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
					const auto& B = rule[i];
					if (Grammar2::IsNonTerminal(B))
					{
						if (std::vector beta(rule.begin() + i + 1, rule.end()); beta.empty())
						{
							for (const auto& f : m_follow[nt])
							{
								if (m_follow[B].insert(f).second)
								{
									changed = true;
								}
							}
						}
						else
						{
							auto firstBeta = GetFirstOfSequence(beta);
							bool betaHasEps = false;

							for (const auto& f : firstBeta)
							{
								if (f.ruleName == EPSILON)
								{
									betaHasEps = true;
								}
								else
								{
									if (m_follow[B].insert(f).second)
									{
										changed = true;
									}
								}
							}

							if (betaHasEps)
							{
								for (const auto& f : m_follow[nt])
								{
									if (m_follow[B].insert(f).second)
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
}

void LLTableBuilder::ValidateLL1(const Grammar2& g)
{
	const auto orderedNonTerminals = g.GetNonTerminals();
	const auto& rulesMap = g.GetRules();

	for (const auto& nt : orderedNonTerminals)
	{
		const auto& rules = rulesMap.at(nt);

		if (rules.size() <= 1)
		{
			continue;
		}

		std::set<RuleItem> seenTerminals;
		for (const auto& rule : rules)
		{
			for (auto guide = GetGuideSet(nt, rule); const auto& c : guide)
			{
				if (seenTerminals.contains(c))
				{
					std::string errorMsg = "Grammar is not LL(1)! ";
					errorMsg += "Conflict found for non terminals '";
					errorMsg += nt.ruleName + "'. The conflict character is '" + c.ruleName + "'.\n";

					throw std::runtime_error(errorMsg);
				}
				seenTerminals.insert(c);
			}
		}
	}
}
void LLTableBuilder::TransformRowsToMap()
{
	m_rows.reserve(m_table.size());

	for (const auto& row : m_table)
	{
		auto currentId = row.id;
		m_rows.emplace(currentId, row);
	}
}
