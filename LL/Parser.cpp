#include "Parser.h"

#include <iomanip>

void PrintRow(const TableRow& row);
void PrintHead();

Parser::Parser(const Grammar2& grammar)
	: m_currToken()
{
	m_table.BuildTable(grammar);
}

bool Parser::ParseInput(const std::string& inputStr)
{
	PrintHead();

	size_t tokenIndex = 0;
	m_rowIndex = 1;
	m_parsedTokens.clear();

	const auto table = m_table.GetTable();
	auto input = TokenizeString(inputStr);

	while (true)
	{
		m_currToken = (tokenIndex < input.size()) ? input[tokenIndex] : RuleItem{ LLTableBuilder::END_OF_FILE, true };

		auto currRow = table.at(m_rowIndex);

		if (!currRow.guideSet.contains(m_currToken) && currRow.leftPart != LLTableBuilder::EPSILON)
		{
			bool rowFound = false;
			for (size_t i = m_rowIndex + 1; i <= table.size(); i++)
			{
				if (table.find(i) == table.end())
				{
					break;
				}

				currRow = table.at(i);
				if (currRow.guideSet.contains(m_currToken))
				{
					rowFound = true;
					m_rowIndex = i;
					break;
				}

				if (currRow.error)
				{
					m_rowIndex = i;
					break;
				}
			}

			if (!rowFound)
			{
				std::cout << "Error: Unexpected token '" << m_currToken.ruleName << "' at index " << tokenIndex << std::endl;
				return false;
			}
		}

		PrintCurrRow();

		if (currRow.stack != 0)
		{
			m_stack.push(currRow.stack);
		}

		if (currRow.shift)
		{
			m_parsedTokens.push_back(m_currToken);
			tokenIndex++;
		}

		if (currRow.next != 0)
		{
			m_rowIndex = currRow.next;
			continue;
		}

		if (!m_stack.empty())
		{
			m_rowIndex = m_stack.top();
			m_stack.pop();
			continue;
		}

		return currRow.end;
	}
}

void Parser::PrintCurrRow(std::ostream& out) const
{
	const auto table = m_table.GetTable();
	if (!table.contains(m_rowIndex))
	{
		return;
	}

	const auto& row = table.at(m_rowIndex);

	std::string guideStr;
	for (const auto& item : row.guideSet)
	{
		guideStr += item.ruleName + " ";
	}

	out << std::left
			  << std::setw(5) << row.id
			  << std::setw(10) << row.leftPart
			  << std::setw(45) << (guideStr.length() > 44 ? guideStr.substr(0, 41) + "..." : guideStr)
			  << std::setw(10) << row.next
			  << std::setw(10) << row.stack
			  << std::setw(10) << (row.error ? "true" : "false")
			  << std::setw(10) << (row.end ? "true" : "false")
			  << std::setw(10) << (row.shift ? "true" : "false")
			  << std::endl;
}
std::vector<RuleItem> Parser::GetParsedStr() const
{
	return m_parsedTokens;
}

RuleItem Parser::GetCurrToken() const
{
	return m_currToken;
}

std::vector<RuleItem> Parser::TokenizeString(const std::string& input)
{
	std::vector<RuleItem> tokens;
	std::stringstream ss(input);
	std::string word;

	while (ss >> word)
	{
		tokens.push_back({ word, true });
	}

	return tokens;
}

void PrintHead()
{
	std::cout << std::left
			  << std::setw(5) << "ID"
			  << std::setw(10) << "Symbol"
			  << std::setw(45) << "Guide Set"
			  << std::setw(10) << "Next"
			  << std::setw(10) << "Stack"
			  << std::setw(10) << "Error"
			  << std::setw(10) << "End"
			  << std::setw(10) << "Shift"
			  << std::endl;
	std::cout << std::string(90, '-') << std::endl;
}