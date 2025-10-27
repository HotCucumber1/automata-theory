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
	bool HasTransition(const State& from, const Input& input) override;
	State GetNextState(const State& fromState, const Input& input) const override;
	std::unique_ptr<Machine> GetMinimized() const override;
	void AddStateOutput(const State& state, const Output& output);
	void AddTransition(const State& from, const Input& input, const State& to);
	Output GetOutputForState(const State& state) const;

	State GetInitialState() const override
	{
		return m_initialState;
	}

private:
	void ConvertFromMealy(MealyMachine& mealy);
	void Clear();
	void RemoveUnreachableStates();

private:
	struct Transition
	{
		State nextState;
		explicit Transition(State next)
			: nextState(std::move(next))
		{
		}
	};

	State m_initialState;
	State m_currentState;
	std::unordered_map<State, Output> m_stateOutputs;
	std::unordered_map<State, std::unordered_map<Input, Transition>> m_transitions;
};
