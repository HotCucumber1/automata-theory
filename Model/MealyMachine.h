#pragma once

#include "Machine.h"
#include <unordered_map>
#include <utility>

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
	State GetNextState(const State& fromState, const Input& input) const override;
	std::unique_ptr<Machine> GetMinimized() const override;

	State GetInitialState() const override
	{
		return m_initialState;
	}

private:
	void ConvertFromMoore(MooreMachine& moore);
	void RemoveUnreachableStates();
	void Clear();

	State m_initialState;
	State m_currentState;
	std::unordered_map<State, std::unordered_map<Input, Transition>> m_transitions;
};
