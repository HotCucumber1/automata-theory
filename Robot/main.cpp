#include <fstream>
#include <iostream>
#include <set>

enum class State
{
	Start,
	Stop,
	Left,
	Right,
	On45,
	HandsUp,
	HandsDown,
	Step,
	BraceOpen,
	BraceClose,
	TurnHead,
	Digit,
	Error,
};

bool IsAlpha(const std::string& string)
{
	for (const auto ch : string)
	{
		if (!std::isdigit(ch))
		{
			return false;
		}
	}
	return true;
}

State WordToState(const std::string& word)
{
	if (word == "start")
		return State::Start;
	if (word == "stop")
		return State::Stop;
	if (word == "left")
		return State::Left;
	if (word == "right")
		return State::Right;
	if (word == "on45")
		return State::On45;
	if (word == "hands_up")
		return State::HandsUp;
	if (word == "hands_down")
		return State::HandsDown;
	if (word == "step_")
		return State::Step;
	if (word == "(")
		return State::BraceOpen;
	if (word == ")")
		return State::BraceClose;
	if (word == "turn_head")
		return State::TurnHead;
	if (IsAlpha(word))
		return State::Digit;

	return State::Error;
}

std::set<State> GetValidStates(const State& state)
{
	switch (state)
	{
	case State::Start:
	case State::HandsUp:
	case State::On45:
		return { State::Left, State::Right, State::On45, State::HandsUp };
	case State::Left:
	case State::Right:
	case State::HandsDown:
	case State::TurnHead:
		return { State::Step, State::TurnHead, State::HandsDown };
	case State::Step:
		return { State::BraceOpen };
	case State::BraceOpen:
		return { State::Digit };
	case State::BraceClose:
		return { State::Left, State::Right, State::On45, State::HandsUp, State::TurnHead };
	case State::Digit:
		return { State::Digit, State::BraceClose };
	case State::Stop:
		return {};
	default:
		throw std::runtime_error("invalid state");
	}
}

int main()
{
	std::ifstream file("input.txt");

	std::set validStates = { State::Start };
	int counter = 0;

	while (file.good())
	{
		std::string word;
		file >> word;

		auto readState = WordToState(word);
		counter++;

		if (readState == State::Stop)
		{
			std::cout << "OK" << std::endl;
			break;
		}
		if (!validStates.contains(readState))
		{
			std::cout << "ERROR" << std::endl;
			break;
		}
		validStates = GetValidStates(readState);
	}
	std::cout << counter << std::endl;
}
