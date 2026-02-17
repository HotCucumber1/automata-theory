#pragma once
#include "Parser.h"
#include "Token.h"

#include <string>

class Lexer
{
public:
	explicit Lexer(const std::string& str);

	explicit Lexer(const std::istream& input);

	Token GetToken();

	bool IsEmpty();

private:
	Token Identifier();

	Token Number();

	Token String();

	Token SpecialChar();

	Robot m_parser;
};
