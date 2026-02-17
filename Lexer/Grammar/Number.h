#pragma once
#include "../Parser.h"

inline bool IsDigit(const char c)
{
	return c >= '0' && c <= '9';
}

inline bool NonZero(const char c)
{
	return c > '0' && c <= '9';
}

inline bool ExpTail(Robot& parser)
{
	if (parser.IsEmpty())
	{
		return true;
	}

	if (IsDigit(parser.GetNext()))
	{
		parser.GetCh();
		return ExpTail(parser);
	}

	return true;
}


inline bool ExpVal(Robot& parser)
{
	return !parser.IsEmpty() && NonZero(parser.GetCh()) && ExpTail(parser);
}

inline bool Exp(Robot& parser)
{
	if (parser.IsEmpty())
	{
		return false;
	}

	const auto sign = parser.GetCh();
	return (sign == '+' || sign == '-') && ExpVal(parser);
}

inline bool ExpPart(Robot& parser, bool& isInteger)
{
	if (parser.IsEmpty())
	{
		return true;
	}

	const auto ch = parser.GetNext();
	if (ch == 'e' || ch == 'E')
	{
		isInteger = false;
		parser.GetCh();
		return Exp(parser);
	}

	return true;
}

inline bool MantissaTail(Robot& parser)
{
	if (parser.IsEmpty())
	{
		return true;
	}

	if (IsDigit(parser.GetNext()))
	{
		parser.GetCh();
		return MantissaTail(parser);
	}

	return true;
}

inline bool Mantissa(Robot& parser)
{
	if (parser.IsEmpty())
	{
		return false;
	}

	return IsDigit(parser.GetCh()) && MantissaTail(parser);
}

inline bool OptMantissa(Robot& parser, bool& isInteger)
{
	if (parser.IsEmpty())
	{
		return true;
	}

	if (parser.GetNext() == '.')
	{
		isInteger = false;
		parser.GetCh();
		return Mantissa(parser);
	}

	return !IsDigit(parser.GetNext());
}

inline bool NumberTail(Robot& parser, bool& isInteger)
{
	if (parser.IsEmpty())
	{
		return true;
	}

	if (IsDigit(parser.GetNext()))
	{
		parser.GetCh();
		return NumberTail(parser, isInteger);
	}

	if (parser.GetNext() == '.')
	{
		isInteger = false;
		parser.GetCh();
		return Mantissa(parser);
	}

	return true;
}

inline bool Number(Robot& parser, bool& isInteger)
{
	if (parser.IsEmpty())
	{
		return false;
	}

	const auto ch = parser.GetNext();
	if (NonZero(ch))
	{
		parser.GetCh();
		return NumberTail(parser, isInteger);
	}

	if (ch == '0')
	{
		parser.GetCh();
		return OptMantissa(parser, isInteger);
	}

	return false;
}


inline bool NumberGrammar(Robot& parser, bool& isInteger)
{
	isInteger = true;
	return Number(parser, isInteger) && ExpPart(parser, isInteger);
}
