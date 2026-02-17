#pragma once
#include <algorithm>
#include <string>
#include <vector>

class Robot
{
public:
	bool Parse(const std::string& input)
	{
		Tokenize(input);
		ResetParser();

		return ParseStartRule() && m_pos == m_tokens.size();
	}

	size_t GetErrorPosition() const
	{
		if (m_errorPos == -1)
		{
			return m_tokens.empty() ? 1 : m_tokens.size() + 1;
		}
		return m_errorPos;
	}

	size_t GetTokenCount() const
	{
		return m_tokens.size();
	}

private:
	std::vector<std::string> m_tokens;
	int m_pos = 0;
	int m_errorPos = -1;

	bool Match(const std::string& expected)
	{
		if (IsAtEnd())
		{
			SetErrorPosition();
			return false;
		}
		if (m_tokens[m_pos] == expected)
		{
			++m_pos;
			return true;
		}
		SetErrorPosition();
		return false;
	}

	bool LookAheadIs(const std::string& token) const
	{
		return m_pos < m_tokens.size() && m_tokens[m_pos] == token;
	}

	bool IsAtEnd() const
	{
		return m_pos >= m_tokens.size();
	}

	void SetErrorPosition()
	{
		if (m_errorPos == -1)
		{
			m_errorPos = m_pos + 1;
		}
	}

	bool ParseDigits()
	{
		if (!MatchNumber())
		{
			return false;
		}
		while (LookAheadIsNumber())
		{
			MatchNumber();
		}
		return true;
	}

	bool MatchNumber()
	{
		if (IsAtEnd())
		{
			SetErrorPosition();
			return false;
		}
		if (IsNumber(m_tokens[m_pos]))
		{
			++m_pos;
			return true;
		}
		SetErrorPosition();
		return false;
	}

	bool LookAheadIsNumber() const
	{
		return m_pos < m_tokens.size() && IsNumber(m_tokens[m_pos]);
	}

	bool ParseStep()
	{
		if (!Match("step_"))
		{
			return false;
		}
		if (!Match("("))
		{
			return false;
		}
		if (!ParseDigits())
		{
			return false;
		}
		if (!Match(")"))
		{
			return false;
		}

		return true;
	}

	bool ParseTurnHead()
	{
		return Match("turn_head");
	}

	bool ParseBasicMovement()
	{
		if (LookAheadIs("left"))
		{
			return Match("left");
		}
		if (LookAheadIs("right"))
		{
			return Match("right");
		}
		if (LookAheadIs("on45"))
		{
			return Match("on45") && ParseBasicMovement();
		}
		if (LookAheadIs("hands_up"))
		{
			return ParseHandsMovement();
		}
		SetErrorPosition();
		return false;
	}

	bool ParseHandsMovement()
	{
		if (!Match("hands_up"))
		{
			return false;
		}
		if (!ParseSequence())
		{
			return false;
		}
		return Match("hands_down");
	}

	bool ParseMovementWithTurns()
	{
		if (!ParseBasicMovement())
		{
			return false;
		}
		while (LookAheadIs("turn_head"))
		{
			if (!ParseTurnHead())
			{
				return false;
			}

			if (!ParseBasicMovement())
			{
				return false;
			}
		}

		return true;
	}

	bool ParseSequence()
	{
		if (!ParseMovementWithTurns())
		{
			return false;
		}
		while (LookAheadIs("step_"))
		{
			if (!ParseStep())
			{
				return false;
			}
			if (!ParseMovementWithTurns())
			{
				return false;
			}
		}
		return true;
	}

	bool ParseStartRule()
	{
		if (!Match("start"))
		{
			return false;
		}
		if (!ParseSequence())
		{
			return false;
		}
		return Match("stop");
	}

	void Tokenize(const std::string& line)
	{
		m_tokens.clear();
		std::string token;

		for (const char ch : line)
		{
			if (std::isspace(static_cast<unsigned char>(ch)))
			{
				AddTokenIfNotEmpty(token);
				token.clear();
			}
			else
			{
				token += ch;
			}
		}
		AddTokenIfNotEmpty(token);
	}

	void AddTokenIfNotEmpty(std::string& token)
	{
		if (!token.empty())
		{
			m_tokens.push_back(token);
			token.clear();
		}
	}

	void ResetParser()
	{
		m_pos = 0;
		m_errorPos = -1;
	}

	static bool IsNumber(const std::string& str)
	{
		return !str.empty() && std::ranges::all_of(str, [](const unsigned char ch) {
			return std::isdigit(ch);
		});
	}
};
