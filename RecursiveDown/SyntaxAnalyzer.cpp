#include "SyntaxAnalyzer.h"

SyntaxAnalyzer::SyntaxAnalyzer(Lexer& lexer, std::ostream& output)
	: m_lexer(lexer)
	, m_output(output)
{
	NextToken();
}

bool SyntaxAnalyzer::Parse()
{
	try
	{
		MainBlock();
		return true;
	}
	catch (const std::exception& e)
	{
		m_output << "Syntax error: " << e.what() << std::endl;
		return false;
	}
}

void SyntaxAnalyzer::Match(const TokenType expectedType)
{
	if (m_currentToke.type == expectedType)
	{
		NextToken();
	}
	else
	{
		const std::string msg = "Unexpected token '" + m_currentToke.value + "'. Line " + std::to_string(m_currentToke.line) + ", position " + std::to_string(m_currentToke.pos) + ". ";
		throw std::runtime_error(msg + TOKEN_NAMES.at(expectedType) + " expected.");
	}
}

void SyntaxAnalyzer::NextToken()
{
	if (!m_lexer.IsEmpty())
	{
		m_currentToke = m_lexer.GetToken();
	}
}

bool SyntaxAnalyzer::Check(const TokenType type) const
{
	return m_currentToke.type == type;
}

void SyntaxAnalyzer::MainBlock()
{
	Match(TokenType::MAIN);
	Body();
	Match(TokenType::END);
	Match(TokenType::OP_DOT);
}

void SyntaxAnalyzer::Body()
{
	Declarations();
	Match(TokenType::BEGIN);
	Statements();
	Match(TokenType::END);
}

void SyntaxAnalyzer::Declarations()
{
	try
	{
		Declaration();

		if (Check(TokenType::SEMICOLON))
		{
			Match(TokenType::SEMICOLON);
			Declarations();
		}
	}
	catch (const std::exception&)
	{
	}
}

void SyntaxAnalyzer::Declaration()
{
	if (Check(TokenType::VAR))
	{
		Var();
	}
	else
	{
		Consts();
	}
}

void SyntaxAnalyzer::Var()
{
	Match(TokenType::VAR);
	IdentifierList();
	Match(TokenType::COLON);
	Type();
}

void SyntaxAnalyzer::IdentifierList()
{
	Match(TokenType::IDENTIFIER);

	if (Check(TokenType::COMMA))
	{
		Match(TokenType::COMMA);
		IdentifierList();
	}
}

void SyntaxAnalyzer::Type()
{
	if (Check(TokenType::TYPE_INT))
	{
		Match(TokenType::TYPE_INT);
	}
	else if (Check(TokenType::TYPE_FLOAT))
	{
		Match(TokenType::TYPE_FLOAT);
	}
	else
	{
		throw std::runtime_error("Expected 'int' or 'float', " + m_currentToke.value + " given");
	}
}

void SyntaxAnalyzer::Consts()
{
	Const();

	if (Check(TokenType::SEMICOLON))
	{
		Match(TokenType::SEMICOLON);
		Consts();
	}
}

void SyntaxAnalyzer::Const()
{
	Match(TokenType::CONST);
	Match(TokenType::IDENTIFIER);
	Match(TokenType::OP_ASSIGN);
	Expr();
}

void SyntaxAnalyzer::Statements()
{
	Statement();

	if (Check(TokenType::SEMICOLON))
	{
		Match(TokenType::SEMICOLON);
		Statements();
	}
}

void SyntaxAnalyzer::Statement()
{
	Assign();
}

void SyntaxAnalyzer::Assign()
{
	Match(TokenType::IDENTIFIER);
	Match(TokenType::OP_ASSIGN);
	Expr();
}

void SyntaxAnalyzer::Expr()
{
	T();

	while (Check(TokenType::OP_PLUS))
	{
		Match(TokenType::OP_PLUS);
		T();
	}
}

void SyntaxAnalyzer::T()
{
	F();

	while (Check(TokenType::OP_MULTI))
	{
		Match(TokenType::OP_MULTI);
		F();
	}
}

void SyntaxAnalyzer::F()
{
	if (Check(TokenType::OP_MINUS))
	{
		Match(TokenType::OP_MINUS);
		F();
	}
	else if (Check(TokenType::BRACKET_BASE_OPEN))
	{
		Match(TokenType::BRACKET_BASE_OPEN);
		Expr();
		Match(TokenType::BRACKET_BASE_CLOSE);
	}
	else if (Check(TokenType::IDENTIFIER))
	{
		Match(TokenType::IDENTIFIER);
	}
	else if (Check(TokenType::INT))
	{
		Match(TokenType::INT);
	}
	else if (Check(TokenType::FLOAT))
	{
		Match(TokenType::FLOAT);
	}
	else
	{
		throw std::runtime_error("Expected '-', '(', identifier, or number");
	}
}
