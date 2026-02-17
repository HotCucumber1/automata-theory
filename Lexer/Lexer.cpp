#include "Lexer.h"
#include "Grammar/Identifier.h"
#include "Grammar/Keywords.h"
#include "Grammar/Number.h"
#include "Grammar/SpecialChar.h"
#include "Grammar/String.h"

#include <iostream>

Lexer::Lexer(const std::string& str)
	: m_parser(str)
{
}

Lexer::Lexer(const std::istream& input)
	: m_parser(input)
{
}

Token Lexer::GetToken()
{
	if (IsEmpty())
	{
		return {
			m_parser.GetCurrentPosition(),
			m_parser.GetCurrentLine(),
			TokenType::ERROR,
		};
	}

	const char ch = m_parser.GetNext();

	if (IsChar(ch))
	{
		return CheckKeywords(Identifier());
	}
	if (IsDigit(ch))
	{
		return Number();
	}
	if (IsQuote(ch))
	{
		return String();
	}
	if (IsSpecialChar(ch))
	{
		return SpecialChar();
	}

	return {
		m_parser.GetCurrentPosition(),
		m_parser.GetCurrentLine(),
		TokenType::ERROR,
	};
}

bool Lexer::IsEmpty()
{
	m_parser.SkipSpaces();
	return m_parser.IsEmpty();
}

Token Lexer::Identifier()
{
	const auto startPos = m_parser.GetCurrentPosition();
	m_parser.ResetBuffer();

	if (!IdentifierGrammar(m_parser))
	{
		return {
			startPos,
			m_parser.GetCurrentLine(),
			TokenType::ERROR,
		};
	}

	return {
		startPos,
		m_parser.GetCurrentLine(),
		TokenType::IDENTIFIER,
		m_parser.GetBuffer(),
	};
}

Token Lexer::Number()
{
	const auto startPos = m_parser.GetCurrentPosition();
	m_parser.ResetBuffer();
	bool isInteger = false;

	if (!NumberGrammar(m_parser, isInteger))
	{
		return {
			startPos,
			m_parser.GetCurrentLine(),
			TokenType::ERROR,
		};
	}

	return {
		startPos,
		m_parser.GetCurrentLine(),
		isInteger
			? TokenType::INT
			: TokenType::FLOAT,
		m_parser.GetBuffer(),
	};
}

Token Lexer::String()
{
	const auto startPos = m_parser.GetCurrentPosition();
	m_parser.ResetBuffer();

	if (!StringGrammar(m_parser))
	{
		return {
			startPos,
			m_parser.GetCurrentLine(),
			TokenType::ERROR,
		};
	}

	return {
		startPos,
		m_parser.GetCurrentLine(),
		TokenType::STRING,
		m_parser.GetBuffer(),
	};
}

Token Lexer::SpecialChar()
{
	const auto startPos = m_parser.GetCurrentPosition();
	m_parser.ResetBuffer();
	const auto tokenType = SpecialCharGrammar(m_parser);

	return {
		startPos,
		m_parser.GetCurrentLine(),
		tokenType,
		m_parser.GetBuffer(),
	};
}
