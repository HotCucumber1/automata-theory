#pragma once

#include "Machine.h"

#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

class MealyMachine;

class MooreMachine final : public Machine
{
public:
	static const Input EPSILON;

	enum class GrammarType
	{
		UNKNOWN,
		RIGHT_LINEAR,
		LEFT_LINEAR,
		MIXED_INVALID
	};

	explicit MooreMachine(State initialState = "");

	explicit MooreMachine(MealyMachine& mealyMachine);

	void FromRegular(const std::string& regular);

	void FromDot(const std::string& fileName) override;

	void FromGrammar(const std::string& fileName);

	void FromRightGrammar(const std::string& fileName);

	void FromLeftGrammar(const std::string& fileName);

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
	struct GrammarComponents
	{
		State startSymbol;
		std::set<State> nonTerminals;
		std::vector<std::pair<State, std::string>> rules;
	};

	struct NFAFragment
	{
		State startState;
		State acceptState;
	};

	using TransitionMap = std::unordered_map<State, std::unordered_map<Input, std::vector<State>>>;

	void ConvertFromMealy(MealyMachine& mealy);

	void Clear();

	void RemoveUnreachableStates();

	std::set<State> EpsilonClosure(const std::set<State>& states) const;

	std::optional<Output> GetConsistentOutput(const std::set<State>& states) const;

	void BuildNFAFromRightGrammar(const GrammarComponents& grammar);

	void BuildNFAFromLeftGrammar(const GrammarComponents& grammar);

	NFAFragment BuildNFAFromReg(const std::string& expr);

	NFAFragment ParseAlternation(const std::string& expr, size_t& pos);

	NFAFragment ParseConcatenation(const std::string& expr, size_t& pos);

	NFAFragment ParseElement(const std::string& expr, size_t& pos);

	NFAFragment ParseAtom(const std::string& expr, size_t& pos);

	NFAFragment GenerateNewStates(const Input& input);

	NFAFragment CreateAlternationNFA(const NFAFragment& a, const NFAFragment& b);

	NFAFragment CreateConcatenationNFA(const NFAFragment& a, const NFAFragment& b);

	NFAFragment CreateStarNFA(const NFAFragment& fragment);

	State GenerateNewState();

	static GrammarComponents ParseGrammarFile(std::ifstream& file);

	static GrammarType DetectGrammarType(const GrammarComponents& grammar);

	State m_initialState;
	State m_currentState;
	std::unordered_map<State, Output> m_stateOutputs;
	TransitionMap m_transitions;
	int m_stateCounter = 0;

	static const State F_STATE;
	static const State S_START;
};