#include "Parser.h"

#include <iomanip>

void PrintRow(const TableRow& row);
void PrintHead();

Parser::Parser(const Grammar& grammar)
{
	m_table.BuildTable(grammar);
}

bool Parser::ParseInput(const std::string& input)
{
	PrintHead();

	size_t chIndex = 0;
	const auto tabel = m_table.GetTable();
	while (true)
	{
		m_currChar = input[chIndex];
		auto currRow = tabel.at(m_rowIndex);

		if (!currRow.guideSet.contains(m_currChar) && currRow.symbol[0] != LLTableBuilder::EPSILON)
		{
			bool rowFound = false;
			for (auto i = m_rowIndex + 1; i < tabel.size(); i++)
			{
				currRow = tabel.at(i);
				if (currRow.guideSet.contains(m_currChar))
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
			m_parsedData += m_currChar;
			chIndex++;
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
	const auto row = m_table.GetTable().at(m_rowIndex);
	std::string guideStr;
	for (const auto c : row.guideSet)
	{
		guideStr += std::string(1, c) + " ";
	}

	out << std::left
			  << std::setw(5) << row.id
			  << std::setw(10) << row.symbol
			  << std::setw(45) << guideStr
			  << std::setw(10) << row.next
			  << std::setw(10) << row.stack
			  << std::setw(10) << (row.error ? "true" : "false")
			  << std::setw(10) << (row.end ? "true" : "false")
			  << std::setw(10) << (row.shift ? "true" : "false")
			  << std::endl;
}
std::string Parser::GetParsedStr() const
{
	return m_parsedData;
}

char Parser::GetCurrChar() const
{
	return m_currChar;
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