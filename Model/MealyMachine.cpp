#include "MealyMachine.h"
#include "MooreMachine.h"
#include <fstream>
#include <queue>
#include <regex>
#include <set>
#include <utility>

MealyMachine::MealyMachine(State initialState)
	: m_currentState(std::move(initialState))
	, m_initialState(initialState)
{
}

MealyMachine::MealyMachine(MooreMachine& mooreMachine)
{
	ConvertFromMoore(mooreMachine);
}

void MealyMachine::FromDot(const std::string& fileName)
{
	std::ifstream file(fileName);
	AssertInputIsOpen(file, fileName);

	const std::regex edgeRegex(R"((\w+)\s*->\s*(\w+)\s*\[label\s*=\s*\"([^/]+)/([^\"]+)\"\]\s*;)");
	const std::regex digraphRegex(R"(digraph\s+(\w+)\s*\{)");
	const std::regex initialStateRegex(R"(\{shape\s*=\s*doublecircle\})");

	State initialState;

	Clear();
	m_transitions.clear();

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

		if (std::regex_search(line, match, edgeRegex))
		{
			State fromState = match[1];
			State toState = match[2];
			Input input = match[3];
			Output output = match[4];

			if (!input.empty() && input.front() == '"' && input.back() == '"')
			{
				input = input.substr(1, input.length() - 2);
			}
			if (!output.empty() && output.front() == '"' && output.back() == '"')
			{
				output = output.substr(1, output.length() - 2);
			}

			AddTransition(fromState, input, toState, output);
			if (line.find("doublecircle") != std::string::npos)
			{
				initialState = fromState;
			}
		}

		if (std::regex_search(line, match, initialStateRegex))
		{
			size_t pos = line.find('[');
			if (pos != std::string::npos)
			{
				State state = line.substr(0, pos);
				state.erase(state.find_last_not_of(" \t") + 1);
				initialState = state;
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

void MealyMachine::SaveToDot(const std::string& fileName)
{
	std::ofstream file(fileName);
	AssertOutputIsOpen(file, fileName);

	file << "digraph MealyMachine {" << std::endl;
	file << "    rankdir=LR;" << std::endl;
	file << "    size=\"8,5\"" << std::endl
		 << std::endl;

	for (const auto& state : m_states)
	{
		file << "    " << state;
		if (state == m_currentState)
		{
			file << " [shape=doublecircle, color=blue]";
		}
		file << ";" << std::endl;
	}
	file << std::endl;

	for (const auto& [fromState, transitions] : m_transitions)
	{
		for (const auto& [input, transition] : transitions)
		{
			file << "    " << fromState << " -> " << transition.nextState
				 << " [label=\"" << input << "/" << transition.output << "\"];" << std::endl;
		}
	}

	file << "}" << std::endl;
}

void MealyMachine::AddTransition(
	const State& from,
	const Input& input,
	const State& to,
	const Output& output)
{
	m_transitions[from].emplace(input, Transition(to, output));

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
	if (std::ranges::find(m_outputs, output) == m_outputs.end())
	{
		m_outputs.push_back(output);
	}
}

bool MealyMachine::HasTransition(const State& from, const Input& input)
{
	const auto stateIt = m_transitions.find(from);
	if (stateIt == m_transitions.end())
	{
		return false;
	}
	return stateIt->second.contains(input);
}

void MealyMachine::ConvertFromMoore(MooreMachine& moore)
{
	Clear();

	State initialState = moore.GetInitialState();
	if (initialState.empty() && !moore.GetStates().empty())
	{
		initialState = moore.GetStates().front();
	}

	if (initialState.empty())
	{
		return;
	}

	m_currentState = initialState;

	std::queue<State> statesToProcess;
	std::set<State> visitedStates;

	statesToProcess.push(initialState);
	visitedStates.insert(initialState);

	const auto mooreInputs = moore.GetInputs();

	while (!statesToProcess.empty())
	{
		State fromState = statesToProcess.front();
		statesToProcess.pop();

		for (const auto& input : mooreInputs)
		{
			if (!moore.HasTransition(fromState, input))
			{
				continue;
			}
			State toState = moore.GetNextState(fromState, input);
			Output output = moore.GetOutputForState(toState);

			AddTransition(fromState, input, toState, output);

			if (!visitedStates.contains(toState))
			{
				visitedStates.insert(toState);
				statesToProcess.push(toState);
			}
		}
	}
}

std::unique_ptr<Machine> MealyMachine::GetMinimized() const
{
	MealyMachine machineToMinimize = *this;

	machineToMinimize.RemoveUnreachableStates();
	if (machineToMinimize.m_states.empty())
	{
		return std::make_unique<MealyMachine>();
	}

	std::map<std::vector<Output>, std::vector<State>> initialGroups;
	for (const auto& state : machineToMinimize.m_states)
	{
		std::vector<Output> outputVector;
		for (const auto& input : machineToMinimize.m_inputs)
		{
			outputVector.push_back(machineToMinimize.GetTransitionOutput(state, input));
		}
		initialGroups[outputVector].push_back(state);
	}

	std::vector<std::vector<State>> partitions;
	for (const auto& pair : initialGroups)
	{
		partitions.push_back(pair.second);
	}

	while (true)
	{
		std::map<State, int> stateToGroupIndex;
		for (int i = 0; i < partitions.size(); ++i)
		{
			for (const auto& state : partitions[i])
			{
				stateToGroupIndex[state] = i;
			}
		}

		std::vector<std::vector<State>> newPartitions;
		bool splitOccurred = false;

		for (const auto& group : partitions)
		{
			if (group.size() <= 1)
			{
				newPartitions.push_back(group);
				continue;
			}

			std::map<std::vector<int>, std::vector<State>> subgroups;
			for (const auto& state : group)
			{
				std::vector<int> transitionSignature;
				for (const auto& input : machineToMinimize.m_inputs)
				{
					State nextState = machineToMinimize.GetNextState(state, input);
					transitionSignature.push_back(stateToGroupIndex.at(nextState));
				}
				subgroups[transitionSignature].push_back(state);
			}

			if (subgroups.size() > 1)
			{
				splitOccurred = true;
			}

			for (const auto& pair : subgroups)
			{
				newPartitions.push_back(pair.second);
			}
		}

		partitions = newPartitions;

		if (!splitOccurred)
		{
			break;
		}
	}

	MealyMachine minimizedMachine;
	std::unordered_map<State, State> oldStateToNewState;

	int newStateCounter = 0;
	for (const auto& group : partitions)
	{
		State newStateName = "S" + std::to_string(newStateCounter++);
		minimizedMachine.m_states.push_back(newStateName);

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
			State oldToState = machineToMinimize.GetNextState(representative, input);
			Output output = machineToMinimize.GetTransitionOutput(representative, input);
			const State& newToState = oldStateToNewState.at(oldToState);

			minimizedMachine.AddTransition(newFromState, input, newToState, output);
		}
	}

	minimizedMachine.m_inputs = machineToMinimize.m_inputs;
	minimizedMachine.m_outputs = machineToMinimize.m_outputs;
	return std::make_unique<MealyMachine>(minimizedMachine);
}

std::string MealyMachine::GetTransitionOutput(const std::string& fromState, const std::string& input) const
{
	return GetTransition(fromState, input).output;
}

MealyMachine::Transition MealyMachine::GetTransition(const State& fromState, const Input& input) const
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

	return {
		inputIt->second.nextState,
		inputIt->second.output
	};
}

Machine::State MealyMachine::GetNextState(const State& fromState, const Input& input) const
{
	return GetTransition(fromState, input).nextState;
}

void MealyMachine::Clear()
{
	m_states.clear();
	m_inputs.clear();
	m_outputs.clear();
	m_transitions.clear();
}

void MealyMachine::RemoveUnreachableStates()
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
	for (const auto& state : m_states)
	{
		if (reachableStates.contains(state))
		{
			newStates.push_back(state);
		}
	}
	m_states = newStates;

	std::unordered_map<State, std::unordered_map<Input, Transition>> newTransitions;
	for (const auto& [from, transitions] : m_transitions)
	{
		if (!reachableStates.contains(from))
		{
			continue;
		}
		newTransitions[from] = transitions;
	}
	m_transitions = newTransitions;
}
