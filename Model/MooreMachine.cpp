#include "MooreMachine.h"
#include "MealyMachine.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <ranges>
#include <regex>
#include <set>

const Machine::Input MooreMachine::EPSILON = "";

MooreMachine::MooreMachine(State initialState)
	: m_initialState(initialState)
	, m_currentState(std::move(initialState))
{
}

MooreMachine::MooreMachine(MealyMachine& mealyMachine)
{
	ConvertFromMealy(mealyMachine);
}

void MooreMachine::AddStateOutput(const State& state, const Output& output)
{
	m_stateOutputs[state] = output;

	if (std::ranges::find(m_states, state) == m_states.end())
	{
		m_states.push_back(state);
	}
	if (std::ranges::find(m_outputs, output) == m_outputs.end())
	{
		m_outputs.push_back(output);
	}
}

void MooreMachine::AddTransition(const State& from, const Input& input, const State& to)
{
	auto& nextStates = m_transitions[from][input];
	if (std::ranges::find(nextStates, to) == nextStates.end())
	{
		nextStates.push_back(to);
	}

	if (std::ranges::find(m_states, from) == m_states.end())
	{
		m_states.push_back(from);
	}
	if (std::ranges::find(m_states, to) == m_states.end())
	{
		m_states.push_back(to);
	}
	if (input != EPSILON && std::ranges::find(m_inputs, input) == m_inputs.end())
	{
		m_inputs.push_back(input);
	}
}

void MooreMachine::FromDot(const std::string& fileName)
{
	std::ifstream file(fileName);
	AssertInputIsOpen(file, fileName);

	const std::regex edgeRegex(R"((\w+)\s*->\s*(\w+)\s*\[label\s*=\s*\"([^"]*)\"\]\s*;)");
	const std::regex outputRegex(R"((\w+)\s*\[label\s*=\s*\"(\w+)\s*\/\s*([^\"]+)\"*)");
	const std::regex stateOutputRegex(R"((\w+)\s*\[output\s*=\s*\"([^\"]+)\"\]\s*;)");
	const std::regex digraphRegex(R"(digraph\s+(\w+)\s*\{)");
	const std::regex initialRegex(R"(\s*(\w+)\s*\[.*shape\s*=\s*doublecircle.*\]\s*;)");

	State initialState;
	Clear();

	std::string line;
	while (std::getline(file, line))
	{
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);

		if (line.empty() || line.substr(0, 2) == "//")
		{
			continue;
		}
		std::smatch match;
		if (std::regex_search(line, match, digraphRegex))
		{
			continue;
		}
		if (std::regex_search(line, match, stateOutputRegex))
		{
			State state = match[1];
			Output output = match[2];
			AddStateOutput(state, output);
			continue;
		}
		if (std::regex_search(line, match, outputRegex))
		{
			State state = match[1];
			Output output = match[3];
			AddStateOutput(state, output);
			continue;
		}
		if (std::regex_search(line, match, edgeRegex))
		{
			State fromState = match[1];
			State toState = match[2];
			Input input = match[3];

			if (!input.empty() && input.front() == '"' && input.back() == '"')
			{
				input = input.substr(1, input.length() - 2);
			}
			if (input == "e")
			{
				input = EPSILON;
			}

			AddTransition(fromState, input, toState);
		}
		else if (std::regex_search(line, match, initialRegex))
		{
			initialState = match[1];
		}
		else if (line.find("[shape=circle]") != std::string::npos || line.find("[shape=doublecircle]") != std::string::npos)
		{
			auto pos = line.find('[');
			if (pos != std::string::npos)
			{
				State state = line.substr(0, pos);
				state.erase(state.find_last_not_of(" \t") + 1);

				if (!m_stateOutputs.contains(state))
				{
					AddStateOutput(state, "default");
				}
				if (line.find("doublecircle") != std::string::npos)
				{
					initialState = state;
				}
			}
		}
	}
	file.close();

	if (!initialState.empty())
	{
		m_initialState = initialState;
		m_currentState = initialState;
	}
	else if (!m_states.empty())
	{
		m_initialState = m_states[0];
		m_currentState = m_states[0];
	}
}

void MooreMachine::SaveToDot(const std::string& fileName)
{
	std::ofstream file(fileName);
	AssertOutputIsOpen(file, fileName);

	file << "digraph MooreMachine {" << std::endl;
	file << "    rankdir=LR;" << std::endl;
	file << "    size=\"8,5\"" << std::endl
		 << std::endl;

	for (const auto& state : m_states)
	{
		file << "    " << state << " [label=\"" << state << "\\n";

		auto outputIt = m_stateOutputs.find(state);
		if (outputIt != m_stateOutputs.end())
		{
			file << outputIt->second;
		}
		else
		{
			file << "none";
		}
		file << "\"";

		if (state == m_initialState)
		{
			file << ", shape=doublecircle";
		}
		else
		{
			file << ", shape=circle";
		}

		file << "];" << std::endl;
	}
	file << std::endl;

	for (const auto& [fromState, transitions] : m_transitions)
	{
		for (const auto& [input, nextStates] : transitions)
		{
			for (const auto& nextState : nextStates)
			{
				std::string label = (input == EPSILON) ? "e" : input;
				file << "    " << fromState << " -> "
					 << nextState
					 << " [label=\"" << label
					 << "\"];" << std::endl;
			}
		}
	}

	file << "}" << std::endl;
}

std::unique_ptr<Machine> MooreMachine::GetMinimized() const
{
	if (!IsDeterministic())
	{
		throw std::runtime_error("Cannot minimize a non-deterministic Moore machine. "
								 "Call GetDeterministic() first.");
	}

	MooreMachine machineToMinimize = *this;
	machineToMinimize.RemoveUnreachableStates();

	if (machineToMinimize.m_states.empty())
	{
		return std::make_unique<MooreMachine>();
	}

	std::map<Output, std::vector<State>> initialGroups;
	for (const auto& state : machineToMinimize.m_states)
	{
		initialGroups[machineToMinimize.GetOutputForState(state)].push_back(state);
	}

	std::vector<std::vector<State>> partitions;
	for (const auto& val : initialGroups | std::views::values)
	{
		partitions.push_back(val);
	}

	partitions = machineToMinimize.BreakForPartitions(partitions);
	MooreMachine minimizedMachine;
	std::unordered_map<State, State> oldStateToNewState;
	int newStateCounter = 0;

	for (const auto& group : partitions)
	{
		State newStateName = "S" + std::to_string(newStateCounter++);
		minimizedMachine.m_states.push_back(newStateName);

		State representative = group.front();
		minimizedMachine.AddStateOutput(newStateName, machineToMinimize.GetOutputForState(representative));

		for (const auto& oldState : group)
		{
			oldStateToNewState[oldState] = newStateName;
		}

		if (std::ranges::find(group, machineToMinimize.m_initialState) != group.end())
		{
			minimizedMachine.m_initialState = newStateName;
			minimizedMachine.m_currentState = newStateName;
		}
	}

	for (const auto& group : partitions)
	{
		State representative = group.front();
		const State& newFromState = oldStateToNewState.at(representative);

		for (const auto& input : machineToMinimize.m_inputs)
		{
			if (!machineToMinimize.HasTransition(representative, input))
			{
				continue;
			}
			State oldToState = machineToMinimize.GetNextState(representative, input);
			const State& newToState = oldStateToNewState.at(oldToState);

			if (!minimizedMachine.HasTransition(newFromState, input))
			{
				minimizedMachine.AddTransition(newFromState, input, newToState);
			}
		}
	}
	minimizedMachine.m_inputs = machineToMinimize.m_inputs;
	minimizedMachine.m_outputs = machineToMinimize.m_outputs;

	return std::make_unique<MooreMachine>(minimizedMachine);
}

void MooreMachine::ConvertFromMealy(MealyMachine& mealy)
{
	Clear();

	State mealyInitialState = mealy.GetInitialState();
	if (mealyInitialState.empty())
	{
		return;
	}

	std::map<std::pair<State, Output>, State> newStatesMap;
	std::queue<std::pair<State, Output>> statesToProcess;

	const Output initialOutput = "eps";
	m_initialState = mealyInitialState;
	m_currentState = mealyInitialState;

	AddStateOutput(m_currentState, initialOutput);
	newStatesMap[{ mealyInitialState, initialOutput }] = m_currentState;
	statesToProcess.emplace(mealyInitialState, initialOutput);

	const auto& mealyInputs = mealy.GetInputs();
	int stateCounter = 0;

	while (!statesToProcess.empty())
	{
		auto [currentMealyState, incomingOutput] = statesToProcess.front();
		statesToProcess.pop();

		State fromMooreState = newStatesMap.at({ currentMealyState, incomingOutput });

		for (const auto& input : mealyInputs)
		{
			if (!mealy.HasTransition(currentMealyState, input))
			{
				continue;
			}
			State nextMealyState = mealy.GetNextState(currentMealyState, input);
			Output transitionOutput = mealy.GetTransitionOutput(currentMealyState, input);

			std::pair nextStatePair = { nextMealyState, transitionOutput };
			State toMooreState;

			if (!newStatesMap.contains(nextStatePair))
			{
				toMooreState = nextMealyState + "_" + std::to_string(stateCounter++);

				newStatesMap[nextStatePair] = toMooreState;
				AddStateOutput(toMooreState, transitionOutput);
				statesToProcess.push(nextStatePair);
			}
			else
			{
				toMooreState = newStatesMap.at(nextStatePair);
			}

			AddTransition(fromMooreState, input, toMooreState);
		}
	}
}

bool MooreMachine::HasTransition(const State& from, const Input& input) const
{
	const auto stateIt = m_transitions.find(from);
	if (stateIt == m_transitions.end())
	{
		return false;
	}
	const auto inputIt = stateIt->second.find(input);
	if (inputIt == stateIt->second.end())
	{
		return false;
	}
	return !inputIt->second.empty();
}

std::vector<Machine::State> MooreMachine::GetNextStates(const State& fromState, const Input& input) const
{
	const auto stateIt = m_transitions.find(fromState);
	if (stateIt == m_transitions.end())
	{
		return {};
	}

	const auto inputIt = stateIt->second.find(input);
	if (inputIt == stateIt->second.end())
	{
		return {};
	}

	return inputIt->second;
}

Machine::State MooreMachine::GetNextState(const State& fromState, const Input& input) const
{
	auto nextStates = GetNextStates(fromState, input);

	if (nextStates.empty())
	{
		throw std::runtime_error("No transition from state: " + fromState + " with input: " + input);
	}
	if (nextStates.size() > 1)
	{
		throw std::runtime_error("Ambiguous transition (non-deterministic) for state: " + fromState + ", input: " + input);
	}
	return nextStates.front();
}

Machine::Output MooreMachine::GetOutputForState(const State& state) const
{
	const auto it = m_stateOutputs.find(state);
	if (it == m_stateOutputs.end())
	{
		throw std::runtime_error("No output defined for state: " + state);
	}
	return it->second;
}

bool MooreMachine::IsDeterministic() const
{
	for (const auto& transitions : m_transitions | std::views::values)
	{
		if (transitions.contains(EPSILON) && !transitions.at(EPSILON).empty())
		{
			return false;
		}

		for (const auto& nextStates : transitions | std::views::values)
		{
			if (nextStates.size() > 1)
			{
				return false;
			}
		}
	}
	return true;
}

void MooreMachine::Clear()
{
	m_states.clear();
	m_inputs.clear();
	m_outputs.clear();
	m_stateOutputs.clear();
	m_transitions.clear();
}

std::unique_ptr<Machine> MooreMachine::GetDeterministic() const
{
	if (IsDeterministic())
	{
		return std::make_unique<MooreMachine>(*this);
	}

	auto dfa = std::make_unique<MooreMachine>();
	std::map<std::set<State>, State> knownStates;
	std::queue<std::set<State>> workQueue;
	int newStateCounter = 0;

	std::set<State> initialSet = EpsilonClosure({ m_initialState });
	auto initialOutputOpt = GetConsistentOutput(initialSet);
	if (!initialOutputOpt.has_value())
	{
		throw std::runtime_error("Non-determinizable: Output conflict in initial state's epsilon closure.");
	}
	const Output& initialOutput = initialOutputOpt.value();

	State newInitialStateName = "S" + std::to_string(newStateCounter++);
	dfa->AddStateOutput(newInitialStateName, initialOutput);
	dfa->m_initialState = newInitialStateName;
	dfa->m_currentState = newInitialStateName;

	knownStates[initialSet] = newInitialStateName;
	workQueue.push(initialSet);

	while (!workQueue.empty())
	{
		std::set<State> currentSet = workQueue.front();
		workQueue.pop();
		State dfaFromState = knownStates.at(currentSet);

		for (const Input& input : m_inputs)
		{
			std::set<State> nextStateSet;

			for (const State& s : currentSet)
			{
				auto nextStates = GetNextStates(s, input);
				nextStateSet.insert(nextStates.begin(), nextStates.end());
			}

			if (nextStateSet.empty())
			{
				continue;
			}

			std::set<State> nextStateClosure = EpsilonClosure(nextStateSet);
			if (nextStateClosure.empty())
			{
				continue;
			}

			State dfaToState;
			if (!knownStates.contains(nextStateClosure))
			{
				auto nextOutputOpt = GetConsistentOutput(nextStateClosure);
				if (!nextOutputOpt.has_value())
				{
					throw std::runtime_error("Non-determinizable: Output conflict in subset for input '" + input + "'");
				}
				const Output& nextOutput = nextOutputOpt.value();

				dfaToState = "S" + std::to_string(newStateCounter++);
				dfa->AddStateOutput(dfaToState, nextOutput);
				knownStates[nextStateClosure] = dfaToState;
				workQueue.push(nextStateClosure);
			}
			else
			{
				dfaToState = knownStates.at(nextStateClosure);
			}
			dfa->AddTransition(dfaFromState, input, dfaToState);
		}
	}

	dfa->m_inputs = m_inputs;

	std::set<Output> uniqueOutputs;
	for (const auto& output : dfa->m_stateOutputs | std::views::values)
	{
		uniqueOutputs.insert(output);
	}
	dfa->m_outputs.assign(uniqueOutputs.begin(), uniqueOutputs.end());

	return dfa;
}

std::set<Machine::State> MooreMachine::EpsilonClosure(const std::set<State>& states) const
{
	std::set<State> closure = states;
	std::queue<State> queue;
	for (const auto& s : states)
	{
		queue.push(s);
	}

	while (!queue.empty())
	{
		State s = queue.front();
		queue.pop();

		auto nextStates = GetNextStates(s, EPSILON);
		for (const auto& nextState : nextStates)
		{
			if (closure.contains(nextState))
			{
				continue;
			}
			closure.insert(nextState);
			queue.push(nextState);
		}
	}
	return closure;
}

std::optional<Machine::Output> MooreMachine::GetConsistentOutput(const std::set<State>& states) const
{
	if (states.empty())
	{
		return std::nullopt;
	}

	std::optional<Output> finalOutput;

	for (const auto& state : states)
	{
		Output currentOutput = GetOutputForState(state);

		if (!finalOutput.has_value())
		{
			finalOutput = currentOutput;
		}
		else if (finalOutput.value() != currentOutput)
		{
			if ((finalOutput.value() == "0" && currentOutput == "1") ||
				(finalOutput.value() == "1" && currentOutput == "0"))
			{
				finalOutput = "1";
			}
			else
			{
				return std::nullopt;
			}
		}
	}
	return finalOutput;
}

void MooreMachine::RemoveUnreachableStates()
{
	if (m_initialState.empty() || m_states.empty())
	{
		Clear();
		return;
	}

	std::set<State> reachableStates;
	std::queue<State> queue;

	std::set<State> initialClosure = EpsilonClosure({ m_initialState });
	for (const auto& state : initialClosure)
	{
		queue.push(state);
		reachableStates.insert(state);
	}

	while (!queue.empty())
	{
		State current = queue.front();
		queue.pop();

		std::vector<Input> allInputs = m_inputs;
		allInputs.push_back(EPSILON);

		for (const auto& input : allInputs)
		{
			if (!HasTransition(current, input))
			{
				continue;
			}
			for (const auto& next : GetNextStates(current, input))
			{
				if (reachableStates.contains(next))
				{
					continue;
				}
				reachableStates.insert(next);
				queue.push(next);
			}
		}
	}

	std::vector<State> newStates;
	std::unordered_map<State, Output> newStateOutputs;
	for (const auto& state : m_states)
	{
		if (!reachableStates.contains(state))
		{
			continue;
		}
		newStates.push_back(state);
		if (!m_stateOutputs.contains(state))
		{
			continue;
		}
		newStateOutputs[state] = m_stateOutputs.at(state);
	}
	m_states = newStates;
	m_stateOutputs = newStateOutputs;

	TransitionMap newTransitions;
	for (const auto& [from, transitions] : m_transitions)
	{
		if (!reachableStates.contains(from))
		{
			continue;
		}

		newTransitions[from] = {};
		for (const auto& [input, nextStates] : transitions)
		{
			for (const auto& next : nextStates)
			{
				if (reachableStates.contains(next))
				{
					newTransitions[from][input].push_back(next);
				}
			}
		}
	}
	m_transitions = newTransitions;
}
