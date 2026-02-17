#pragma once
#include "../Parser.h"
#include "../Token.h"

inline TokenType SpecialCharGrammar(Robot& parser)
{
	const auto it = SPECIAL_CHARS.find(parser.GetNext());

	parser.GetCh();
	if (parser.IsEmpty())
	{
		return it->second;
	}

	const auto doubledIt = SPECIAL_CHARS_DOUBLE.find(it->first);
	if (doubledIt == SPECIAL_CHARS_DOUBLE.end() || doubledIt->second.first != parser.GetNext())
	{
		return it->second;
	}

	parser.GetCh();
	return doubledIt->second.second;
}

inline bool IsSpecialChar(const char c)
{
	return SPECIAL_CHARS.contains(c);
}