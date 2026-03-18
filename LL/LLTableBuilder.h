#pragma once
#include "Grammar/Grammar.h"

#include <map>
#include <unordered_map>

struct TableRow
{
	size_t id;
	std::string symbol;
	std::set<char> guideSet;
	size_t next;
	size_t stack;
	bool error;
	bool end;
	bool shift;
};

class LLTableBuilder
{
public:
	static constexpr char EPSILON = '#';
	static constexpr char END_OF_FILE = '$';

	void BuildTable(const Grammar& g);

	void PrintTable() const;

	std::unordered_map<size_t, TableRow> GetTable() const;

private:
	std::set<char> GetFirstOfSequence(const std::string& seq);

	std::set<char> GetGuideSet(char lhs, const std::string& rhs);

	std::set<char> GetGuideForNT(char X);

	void BuildFirstAndFollow(const Grammar& g);

	void ValidateLL1(const Grammar& g);

	void TransformRowsToMap();

private:
	std::map<char, std::set<char>> m_first;
	std::map<char, std::set<char>> m_follow;
	std::vector<TableRow> m_table;
	std::unordered_map<size_t, TableRow> m_rows;
};
