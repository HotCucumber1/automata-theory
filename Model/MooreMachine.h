#pragma once

#include "Machine.h"
#include <unordered_map>
#include <utility>
#include <vector>

class MealyMachine;

class MooreMachine : public Machine
{
public:
	explicit MooreMachine(State initialState = "");
	explicit MooreMachine(MealyMachine& mealyMachine);
	void FromDot(const std::string& fileName) override;
	void SaveToDot(const std::string& fileName) override;
	void AddStateOutput(const State& state, const Output& output);
	void AddTransition(const State& from, const Input& input, const State& to);
	bool HasTransition(const State& from, const Input& input) override;
	State GetNextState(const State& fromState, const Input& input) const;
	Output GetOutputForState(const State& state) const;

private:
	void ConvertFromMealy(MealyMachine& mealy);
	void Clear();

private:
	struct Transition
	{
		State nextState;
		explicit Transition(State next)
			: nextState(std::move(next))
		{
		}
	};

	State m_currentState;
	std::unordered_map<State, Output> m_stateOutputs;
	std::unordered_map<State, std::unordered_map<Input, Transition>> m_transitions;
};
