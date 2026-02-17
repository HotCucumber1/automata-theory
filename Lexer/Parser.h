#pragma once
#include <sstream>
#include <string>
#include <vector>

class Robot
{
public:
	explicit Robot(const std::string& string);

	explicit Robot(const std::istream& input);

	char GetCh();

	void Seek(size_t pos);

	char GetNext();

	int GetCurrentPosition() const;

	int GetCurrentLine() const;

	bool IsEmpty() const;

	void ResetBuffer();

	void SkipSpaces();

	std::string GetBuffer() const;

private:
	std::stringstream m_input;
	int m_currentPosition = 1;
	int m_currentLine = 1;
	int m_globalPosition = 1;
	std::string m_buffer;
	bool m_toNextLine = false;
};
