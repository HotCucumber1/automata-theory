#pragma once
#include "Grammar/Grammar.h"

#include <map>

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
	void BuildTable(const Grammar& g);

	void PrintTable() const;

private:
	std::set<char> GetFirstOfSequence(const std::string& seq);

	std::set<char> GetGuideSet(char lhs, const std::string& rhs);

	std::set<char> GetGuideForNT(char X);

	void BuildFirstAndFollow(const Grammar& g);

	void ValidateLL1(const Grammar& g);

private:
	std::map<char, std::set<char>> first;
	std::map<char, std::set<char>> follow;
	std::vector<TableRow> table;
};
