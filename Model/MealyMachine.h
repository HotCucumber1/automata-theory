#pragma once

#include "Machine.h"

#include <set>
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

	static const Input EPSILON;

	explicit MealyMachine(State initialState = "");
	explicit MealyMachine(MooreMachine& mooreMachine);

	void FromDot(const std::string& fileName) override;
	void SaveToDot(const std::string& fileName) override;
	bool HasTransition(const State& from, const Input& input) const override;
	std::unique_ptr<Machine> GetMinimized() const override;
	State GetNextState(const State& fromState, const Input& input) const override;

	void AddTransition(
		const State& from,
		const Input& input,
		const State& to,
		const Output& output);

	std::vector<Transition> GetTransitions(const State& from, const Input& input) const;
	Transition GetTransition(const State& fromState, const Input& input) const;
	Output GetTransitionOutput(const State& fromState, const Input& input) const;

	bool IsDeterministic() const;

	std::unique_ptr<Machine> GetDeterministic() const;

	State GetInitialState() const override
	{
		return m_initialState;
	}

private:
	using TransitionMap = std::unordered_map<State, std::unordered_map<Input, std::vector<Transition>>>;

	void ConvertFromMoore(MooreMachine& moore);
	void RemoveUnreachableStates();
	void Clear();

	std::set<State> EpsilonClosure(const std::set<State>& states) const;

	State m_initialState;
	State m_currentState;
	TransitionMap m_transitions;
};
