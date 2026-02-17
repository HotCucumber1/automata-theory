#pragma once
#include "../Parser.h"

inline bool IsChar(const char ch)
{
	return ch == '_' or std::isalpha(ch);
}

inline bool IdentifierChar(Robot& parser)
{
	const auto ch = parser.GetNext();
	return IsChar(ch);
}

inline bool IdentifierPart(Robot& parser)
{
	if (IdentifierChar(parser))
	{
		parser.GetCh();
		return true;
	}

	return std::isdigit(parser.GetCh());
}

inline bool IdentifierTail(Robot& parser)
{
	if (parser.IsEmpty())
	{
		return true;
	}

	const auto pos = parser.GetCurrentPosition();
	if (!IdentifierPart(parser))
	{
		parser.Seek(pos);
		return true;
	}

	return IdentifierTail(parser);
}

inline bool IdentifierGrammar(Robot& parser)
{
	if (parser.IsEmpty())
	{
		return false;
	}

	return IdentifierChar(parser) && IdentifierTail(parser);
}