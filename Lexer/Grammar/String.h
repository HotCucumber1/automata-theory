#pragma once
#include "../Parser.h"

inline bool IsQuote(const char c)
{
	return c == '\'' || c == '"';
}

inline bool StringGrammar(Robot& parser)
{
	if (!IsQuote(parser.GetCh()) or parser.IsEmpty())
	{
		return false;
	}


	while (!IsQuote(parser.GetCh()))
	{
		if (parser.IsEmpty())
		{
			return false;
		}
	}

	return true;
}