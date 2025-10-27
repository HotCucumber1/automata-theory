#include "MooreMachine.h"
#include "MealyMachine.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <regex>
#include <set>

MooreMachine::MooreMachine(State initialState)
	: m_currentState(std::move(initialState))
	, m_initialState(initialState)
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
	m_transitions[from].emplace(input, Transition(to));

	if (std::ranges::find(m_states, from) == m_states.end())
	{
		m_states.push_back(from);
	}
	if (std::ranges::find(m_states, to) == m_states.end())
	{
		m_states.push_back(to);
	}
	if (std::ranges::find(m_inputs, input) == m_inputs.end())
	{
		m_inputs.push_back(input);
	}
}

void MooreMachine::FromDot(const std::string& fileName)
{
	std::ifstream file(fileName);
	AssertInputIsOpen(file, fileName);

	const std::regex edgeRegex(R"((\w+)\s*->\s*(\w+)\s*\[label\s*=\s*\"([^\"]+)\"\]\s*;)");
	const std::regex outputRegex(R"((\w+)\s*\[label\s*=\s*\"(\w+)\s*\/\s*([^\"]+)\"*)");
	const std::regex stateOutputRegex(R"((\w+)\s*\[output\s*=\s*\"([^\"]+)\"\]\s*;)");
	const std::regex digraphRegex(R"(digraph\s+(\w+)\s*\{)");

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

			AddTransition(fromState, input, toState);
			if (line.find("doublecircle") != std::string::npos)
			{
				initialState = fromState;
			}
		}

		if (line.find("[shape=circle]") != std::string::npos || line.find("[shape=doublecircle]") != std::string::npos)
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
	if (!initialState.empty())
	{
		m_currentState = initialState;
	}
	else if (!m_states.empty())
	{
		m_currentState = m_states[0];
	}
}

void MooreMachine::SaveToDot(const std::string& fileName)
{
	std::ofstream file(fileName);
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot create file: " + fileName);
	}

	file << "digraph MooreMachine {" << std::endl;
	file << "    rankdir=LR;" << std::endl;
	file << "    size=\"8,5\"" << std::endl
		 << std::endl;

	for (const auto& state : m_states)
	{
		file << "    " << state << " [label=\"" << state << "\\noutput: ";

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

		if (state == m_currentState)
		{
			file << ", shape=doublecircle, color=blue";
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
		for (const auto& [input, transition] : transitions)
		{
			file << "    " << fromState << " -> "
				 << transition.nextState
				 << " [label=\"" << input
				 << "\"];" << std::endl;
		}
	}

	file << "}" << std::endl;
}

std::unique_ptr<Machine> MooreMachine::GetMinimized() const
{
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
	for (const auto& pair : initialGroups)
	{
		partitions.push_back(pair.second);
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

		if (std::ranges::find(group, machineToMinimize.m_currentState) != group.end())
		{
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
			minimizedMachine.AddTransition(newFromState, input, newToState);
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
	m_currentState = mealyInitialState;

	AddStateOutput(m_currentState, initialOutput);
	newStatesMap[{ mealyInitialState, initialOutput }] = m_currentState;
	statesToProcess.emplace(mealyInitialState, initialOutput);

	const auto mealyInputs = mealy.GetInputs();
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

bool MooreMachine::HasTransition(const State& from, const Input& input)
{
	const auto stateIt = m_transitions.find(from);
	if (stateIt == m_transitions.end())
	{
		return false;
	}
	return stateIt->second.contains(input);
}

Machine::State MooreMachine::GetNextState(const State& fromState, const Input& input) const
{
	const auto stateIt = m_transitions.find(fromState);
	if (stateIt == m_transitions.end())
	{
		throw std::runtime_error("No transitions from state: " + fromState);
	}

	const auto inputIt = stateIt->second.find(input);
	if (inputIt == stateIt->second.end())
	{
		throw std::runtime_error("No transition for input: " + input);
	}

	return inputIt->second.nextState;
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

void MooreMachine::Clear()
{
	m_states.clear();
	m_inputs.clear();
	m_outputs.clear();
	m_stateOutputs.clear();
	m_transitions.clear();
}

void MooreMachine::RemoveUnreachableStates()
{
	if (m_currentState.empty() || m_states.empty())
	{
		return;
	}

	std::set<State> reachableStates;
	std::queue<State> q;

	q.push(m_currentState);
	reachableStates.insert(m_currentState);

	while (!q.empty())
	{
		State current = q.front();
		q.pop();

		if (!m_transitions.contains(current))
		{
			continue;
		}
		for (const auto& input : m_inputs)
		{
			if (!HasTransition(current, input))
			{
				continue;
			}
			State next = GetNextState(current, input);
			if (!reachableStates.contains(next))
			{
				reachableStates.insert(next);
				q.push(next);
			}
		}
	}

	std::vector<State> newStates;
	std::unordered_map<State, Output> newStateOutputs;
	for (const auto& state : m_states)
	{
		if (reachableStates.contains(state))
		{
			newStates.push_back(state);
			newStateOutputs[state] = m_stateOutputs.at(state);
		}
	}
	m_states = newStates;
	m_stateOutputs = newStateOutputs;

	std::unordered_map<State, std::unordered_map<Input, Transition>> newTransitions;
	for (const auto& [from, transitions] : m_transitions)
	{
		if (reachableStates.contains(from))
		{
			newTransitions[from] = transitions;
		}
	}
	m_transitions = newTransitions;
}
