#pragma once

#include "Machine.h"

#include <optional>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

class MealyMachine;

class MealyMachine;

class MooreMachine : public Machine
{
public:
	static const Input EPSILON;

	explicit MooreMachine(State initialState = "");
	explicit MooreMachine(MealyMachine& mealyMachine);
	void FromDot(const std::string& fileName) override;
	void SaveToDot(const std::string& fileName) override;
	bool HasTransition(const State& from, const Input& input) const override;
	State GetNextState(const State& fromState, const Input& input) const override;
	std::unique_ptr<Machine> GetMinimized() const override;

	void AddStateOutput(const State& state, const Output& output);
	void AddTransition(const State& from, const Input& input, const State& to);
	Output GetOutputForState(const State& state) const;

	std::vector<State> GetNextStates(const State& fromState, const Input& input) const;
	bool IsDeterministic() const;

	std::unique_ptr<Machine> GetDeterministic() const;

	State GetInitialState() const override
	{
		return m_initialState;
	}

private:
	using TransitionMap = std::unordered_map<State, std::unordered_map<Input, std::vector<State>>>;
	void ConvertFromMealy(MealyMachine& mealy);
	void Clear();
	void RemoveUnreachableStates();

	std::set<State> EpsilonClosure(const std::set<State>& states) const;
	std::optional<Output> GetConsistentOutput(const std::set<State>& states) const;

	State m_initialState;
	State m_currentState;
	std::unordered_map<State, Output> m_stateOutputs;

	TransitionMap m_transitions;
};