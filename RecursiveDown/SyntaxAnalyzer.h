#pragma once
#include "../Lexer/Lexer.h"

#include <iostream>

/**
<mainblock> -> main <body> end.
<body> -> <decls>; begin <sts> end
<decls> -> <decl> | <decl> ; <decls>
<decl> -> <var> | <consts>
<var> -> var <id> : <type>
<id> -> id | id , <id>
<type> -> int | float
<consts> -> <const> | <const> ; <consts>
<const> -> id = <expr>
<sts> -> <st> | <st> ; <sts>
<st> -> <assign>
<assign> -> id := <expr>
<expr> -> <exp> + <T> | <T>
<T> -> <T> * <T> | <F>
<F> -> -<F> | (<expr>) | id | <number>
*/
class SyntaxAnalyzer
{
public:
	explicit SyntaxAnalyzer(Lexer& lexer, std::ostream& output = std::cout);

	bool Parse();

private:
	void Match(TokenType expectedType);

	void NextToken();

	bool Check(TokenType type) const;

	void MainBlock();

	void Body();

	void Declarations();

	void Declaration();

	void Var();

	void IdentifierList();

	void Type();

	void Consts();

	void Const();

	void Statements();

	void Statement();

	void Assign();

	void Expr();

	void T();

	void F();

private:
	Lexer& m_lexer;
	Token m_currentToke;
	std::ostream& m_output;
};
