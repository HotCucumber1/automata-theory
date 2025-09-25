#pragma once

#include "Machine.h"
#include <unordered_map>
#include <utility>
#include <vector>

class MooreMachine;

class MealyMachine : public Machine
{
public:
	struct Transition
	{
		State nextState;
		Output output;

		Transition(State next, Output out)
			: nextState(std::move(next))
			, output(std::move(out))
		{
		}
	};

public:
	explicit MealyMachine(State initialState = "");
	explicit MealyMachine(MooreMachine& mooreMachine);
	void FromDot(const std::string& fileName) override;
	void SaveToDot(const std::string& fileName) override;
	void AddTransition(
		const State& from,
		const Input& input,
		const State& to,
		const Output& output);

	bool HasTransition(const State& from, const Input& input) override;
	Transition GetTransition(const State& fromState, const Input& input) const;
	Output GetTransitionOutput(const State& fromState, const Input& input) const;
	State GetNextState(const State& fromState, const Input& input) const;

private:
	void ConvertFromMoore(MooreMachine& moore);
	void Clear();

private:
	State m_currentState;

	std::unordered_map<State, std::unordered_map<Input, Transition>> m_transitions;
};
