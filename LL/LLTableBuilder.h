#pragma once

#include "Grammar/Grammar2.h"

#include <map>
#include <set>
#include <unordered_map>

struct TableRow
{
	size_t id;
	std::string leftPart;
	std::set<RuleItem> guideSet;
	size_t next;
	size_t stack;
	bool error;
	bool end;
	bool shift;
};

class LLTableBuilder
{
public:
	static constexpr std::string EPSILON = "#";
	static constexpr std::string END_OF_FILE = "$";

	void BuildTable(const Grammar2& g);

	void PrintTable() const;

	std::unordered_map<size_t, TableRow> GetTable() const;

private:
	std::set<RuleItem> GetFirstOfSequence(const std::vector<RuleItem>& seq);

	std::set<RuleItem> GetGuideSet(const RuleItem& lhs, const std::vector<RuleItem>& rhs);

	std::set<RuleItem> GetGuideForNT(const RuleItem& X);

	void BuildFirstAndFollow(const Grammar2& g);

	void ValidateLL1(const Grammar2& g);

	void TransformRowsToMap();

private:
	std::map<RuleItem, std::set<RuleItem>> m_first;
	std::map<RuleItem, std::set<RuleItem>> m_follow;
	std::vector<TableRow> m_table;
	std::unordered_map<size_t, TableRow> m_rows;
};
