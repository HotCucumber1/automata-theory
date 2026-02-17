#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> tokens;
int pos = 0;
int errorPos = -1;

bool ParseRule6();
bool ParseRule4();
bool ParseRule5();
bool ParseRule3();
bool ParseRule2();
bool ParseRule1();
bool ParseStart();

bool Match(const std::string& expected)
{
	if (pos >= tokens.size())
	{
		if (errorPos == -1)
		{
			errorPos = pos + 1;
		}
		return false;
	}
	if (tokens[pos] == expected)
	{
		++pos;
		return true;
	}
	if (errorPos == -1)
	{
		errorPos = pos + 1;
	}
	return false;
}

bool LookAheadIs(const std::string& s)
{
	return pos < tokens.size() && tokens[pos] == s;
}

bool ParseRule6()
{
	if (!Match("digt"))
		return false;
	while (LookAheadIs("digt"))
	{
		Match("digt");
	}
	return true;
}

bool ParseRule4()
{
	if (!Match("step_"))
		{return false;}
	if (!Match("("))
		return false;
	if (!ParseRule6())
		return false;
	if (!Match(")"))
		return false;
	return true;
}

bool ParseRule5()
{
	return Match("turn_head");
}

bool ParseRule3()
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
		if (!Match("on45"))
		{
			return false;
		}
		return ParseRule3();
	}
	if (LookAheadIs("hands_up"))
	{
		if (!Match("hands_up"))
		{
			return false;
		}
		if (!ParseRule1())
		{
			return false;
		}
		if (!Match("hands_down"))
		{
			return false;
		}
		return true;
	}
	if (errorPos == -1)
	{
		errorPos = pos + 1;
	}
	return false;
}

bool ParseRule2()
{
	if (!ParseRule3())
	{
		return false;
	}
	while (LookAheadIs("turn_head"))
	{
		if (!ParseRule5())
		{
			return false;
		}
		if (!ParseRule3())
		{
			return false;
		}
	}
	return true;
}

bool ParseRule1()
{
	if (!ParseRule2())
	{
		return false;
	}
	while (LookAheadIs("step_"))
	{
		if (!ParseRule4())
		{
			return false;
		}
		if (!ParseRule2())
		{
			return false;
		}
	}
	return true;
}

bool ParseStart()
{
	if (!Match("start"))
	{
		return false;
	}
	if (!ParseRule1())
	{
		return false;
	}
	if (!Match("stop"))
	{
		return false;
	}
	return true;
}

int main()
{
	std::ifstream file("input.txt");
	std::string line;
	std::getline(file, line);
	file.close();

	tokens.clear();
	std::string token;
	for (const char ch : line)
	{
		if (std::isspace(static_cast<unsigned char>(ch)))
		{
			if (!token.empty())
			{
				tokens.push_back(token);
				token.clear();
			}
		}
		else
		{
			token += ch;
		}
	}
	if (!token.empty())
	{
		tokens.push_back(token);
	}

	pos = 0;
	errorPos = -1;

	bool ok = ParseStart() && pos == tokens.size();

	std::ofstream fout("output.txt");
	if (ok)
	{
		fout << "OK\n";
		fout << tokens.size() << "\n";
	}
	else
	{
		fout << "ERROR\n";
		if (errorPos == -1)
		{
			errorPos = tokens.empty()
				? 1
				: tokens.size() + 1;
		}
		fout << errorPos << "\n";
	}
	fout.close();

	return 0;
}