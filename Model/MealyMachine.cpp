#include "MealyMachine.h"
#include "MooreMachine.h"
#include <fstream>
#include <queue>
#include <ranges>
#include <regex>
#include <set>
#include <utility>

const Machine::Input MealyMachine::EPSILON;

std::vector<std::vector<MealyMachine::State>> RefinePartitions(
	const MealyMachine& machine,
	std::vector<std::vector<Machine::State>> initialPartitions);
std::vector<std::vector<MealyMachine::State>> CreateInitialPartitions(const MealyMachine& machine);
std::unique_ptr<MealyMachine> BuildMinimizedMachine(
	const MealyMachine& machine,
	const std::vector<std::vector<Machine::State>>& finalPartitions);

MealyMachine::MealyMachine(State initialState)
	: m_initialState(initialState)
	, m_currentState(std::move(initialState))
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

	const std::regex edgeRegex(R"((\w+)\s*->\s*(\w+)\s*\[label\s*=\s*\"([^/]*)/([^\"]+)\"\]\s*;)");
	const std::regex digraphRegex(R"(digraph\s+(\w+)\s*\{)");
	const std::regex initialStateRegex(R"(\s*(\w+)\s*\[.*shape\s*=\s*doublecircle.*\]\s*;)");

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
		}
		else if (std::regex_search(line, match, initialStateRegex))
		{
			initialState = match[1];
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
		if (state == m_initialState)
		{
			file << " [shape=doublecircle, color=blue]";
		}
		file << ";" << std::endl;
	}
	file << std::endl;

	for (const auto& [fromState, transitions] : m_transitions)
	{
		for (const auto& [input, transitionList] : transitions)
		{
			for (const auto& transition : transitionList)
			{
				file << "    " << fromState << " -> " << transition.nextState
					 << " [label=\"" << (input == EPSILON ? "E" : input)
					 << "/" << transition.output << "\"];" << std::endl;
			}
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
	m_transitions[from][input].emplace_back(to, output);

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
	if (std::ranges::find(m_outputs, output) == m_outputs.end())
	{
		m_outputs.push_back(output);
	}
}

bool MealyMachine::HasTransition(const State& from, const Input& input) const
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

std::vector<MealyMachine::Transition> MealyMachine::GetTransitions(const State& fromState, const Input& input) const
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
	MealyMachine machineToMinimize;
	if (!this->IsDeterministic())
	{
		std::unique_ptr<Machine> dmmPtr = this->GetDeterministic();
		auto* deterministicMachine = dynamic_cast<MealyMachine*>(dmmPtr.get());
		if (!deterministicMachine)
		{
			throw std::runtime_error("Failed to cast deterministic machine for minimization.");
		}
		machineToMinimize = *deterministicMachine;
	}
	else
	{
		machineToMinimize = *this;
	}

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
			if (machineToMinimize.HasTransition(state, input))
			{
				outputVector.push_back(machineToMinimize.GetTransitionOutput(state, input));
			}
			else
			{
				outputVector.emplace_back("");
			}
		}
		initialGroups[outputVector].push_back(state);
	}

	std::vector<std::vector<State>> partitions;
	for (const auto& val : initialGroups | std::views::values)
	{
		partitions.push_back(val);
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
					if (machineToMinimize.HasTransition(state, input))
					{
						State nextState = machineToMinimize.GetNextState(state, input);
						transitionSignature.push_back(stateToGroupIndex.at(nextState));
					}
					else
					{
						transitionSignature.push_back(-1);
					}
				}
				subgroups[transitionSignature].push_back(state);
			}

			if (subgroups.size() > 1)
			{
				splitOccurred = true;
			}

			for (const auto& val : subgroups | std::views::values)
			{
				newPartitions.push_back(val);
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
			Output output = machineToMinimize.GetTransitionOutput(representative, input);
			const State& newToState = oldStateToNewState.at(oldToState);

			if (!minimizedMachine.HasTransition(newFromState, input))
			{
				minimizedMachine.AddTransition(newFromState, input, newToState, output);
			}
		}
	}

	minimizedMachine.m_inputs = machineToMinimize.m_inputs;
	std::set<Output> uniqueOutputs;
	for (const auto& map : minimizedMachine.m_transitions | std::views::values)
	{
		for (const auto& transList : map | std::views::values)
		{
			uniqueOutputs.insert(transList.front().output);
		}
	}
	minimizedMachine.m_outputs.assign(uniqueOutputs.begin(), uniqueOutputs.end());
	return std::make_unique<MealyMachine>(minimizedMachine);
}

std::string MealyMachine::GetTransitionOutput(const std::string& fromState, const std::string& input) const
{
	return GetTransition(fromState, input).output;
}

MealyMachine::Transition MealyMachine::GetTransition(const State& fromState, const Input& input) const
{
	auto transitions = GetTransitions(fromState, input);
	if (transitions.empty())
	{
		throw std::runtime_error("No transition for state: " + fromState + ", input: " + input);
	}
	if (transitions.size() > 1)
	{
		throw std::runtime_error("Ambiguous transition (non-deterministic) for state: " + fromState + ", input: " + input);
	}
	return transitions.front();
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
	if (m_initialState.empty() || m_states.empty())
	{
		Clear();
		return;
	}

	std::set<State> reachableStates;
	std::queue<State> queue;

	queue.push(m_initialState);
	reachableStates.insert(m_initialState);

	while (!queue.empty())
	{
		State current = queue.front();
		queue.pop();

		if (!m_transitions.contains(current))
		{
			continue;
		}

		for (const auto& transitionList : m_transitions.at(current) | std::views::values)
		{
			for (const auto& trans : transitionList)
			{
				if (reachableStates.contains(trans.nextState))
				{
					continue;
				}
				reachableStates.insert(trans.nextState);
				queue.push(trans.nextState);
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

	std::unordered_map<State, std::unordered_map<Input, std::vector<Transition>>> newTransitions;
	for (const auto& [from, transitions] : m_transitions)
	{
		if (!reachableStates.contains(from))
		{
			continue;
		}
		newTransitions[from] = transitions;
	}

	for (auto& transitions : newTransitions | std::views::values)
	{
		for (auto& transitionList : transitions | std::views::values)
		{
			std::vector<Transition> newTransitionList;
			for (const auto& trans : transitionList)
			{
				if (!reachableStates.contains(trans.nextState))
				{
					continue;
				}
				newTransitionList.push_back(trans);
			}
			transitionList = newTransitionList;
		}
	}

	m_transitions = newTransitions;
}

bool MealyMachine::IsDeterministic() const
{
	for (const auto& transitions : m_transitions | std::views::values)
	{
		if (transitions.contains(EPSILON))
		{
			return false;
		}

		for (const auto& transitionList : transitions | std::views::values)
		{
			if (transitionList.size() > 1)
			{
				return false;
			}
		}
	}
	return true;
}

std::string StateSetToString(const std::set<MealyMachine::State>& stateSet)
{
	std::stringstream ss;
	bool first = true;
	for (const auto& state : stateSet)
	{
		if (!first)
		{
			ss << "_";
		}
		ss << state;
		first = false;
	}
	return ss.str();
}

std::unique_ptr<Machine> MealyMachine::GetDeterministic() const
{
	if (IsDeterministic())
	{
		return std::make_unique<MealyMachine>(*this);
	}

	auto deterministicMachine = std::make_unique<MealyMachine>();
	std::map<std::set<State>, State> knownStates;
	std::queue<std::set<State>> workQueue;
	int newStateCounter = 0;

	std::set<State> initialClosure = EpsilonClosure({ m_initialState });
	State newInitialStateName = "S" + std::to_string(newStateCounter++);

	deterministicMachine->m_initialState = newInitialStateName;
	deterministicMachine->m_currentState = newInitialStateName;
	deterministicMachine->m_states.push_back(newInitialStateName);

	knownStates[initialClosure] = newInitialStateName;
	workQueue.push(initialClosure);

	while (!workQueue.empty())
	{
		std::set<State> currentSet = workQueue.front();
		workQueue.pop();
		State currentNewStateName = knownStates.at(currentSet);

		for (const Input& input : m_inputs)
		{
			std::set<State> nextStateSet;
			std::optional<Output> transitionOutput;

			for (const State& s : currentSet)
			{
				auto transitions = GetTransitions(s, input);
				for (const auto& trans : transitions)
				{
					if (!transitionOutput.has_value())
					{
						transitionOutput = trans.output;
					}
					else if (transitionOutput.value() != trans.output)
					{
						throw std::runtime_error("Non-determinizable: Output mismatch for input '" + input + "' from states in set " + currentNewStateName);
					}
					nextStateSet.insert(trans.nextState);
				}
			}

			if (!transitionOutput.has_value())
			{
				continue;
			}

			std::set<State> nextStateClosure = EpsilonClosure(nextStateSet);
			if (nextStateClosure.empty())
			{
				continue;
			}

			State nextNewStateName;
			if (!knownStates.contains(nextStateClosure))
			{
				nextNewStateName = "S" + std::to_string(newStateCounter++);
				knownStates[nextStateClosure] = nextNewStateName;
				workQueue.push(nextStateClosure);
				deterministicMachine->m_states.push_back(nextNewStateName);
			}
			else
			{
				nextNewStateName = knownStates.at(nextStateClosure);
			}

			deterministicMachine->AddTransition(
				currentNewStateName,
				input,
				nextNewStateName,
				transitionOutput.value());
		}
	}
	deterministicMachine->m_inputs = m_inputs;

	std::set<Output> uniqueOutputs;
	for (const auto& map : deterministicMachine->m_transitions | std::views::values)
	{
		for (const auto& transList : map | std::views::values)
		{
			for (const auto& trans : transList)
			{
				uniqueOutputs.insert(trans.output);
			}
		}
	}
	deterministicMachine->m_outputs.assign(uniqueOutputs.begin(), uniqueOutputs.end());
	return deterministicMachine;
}

std::set<MealyMachine::State> MealyMachine::EpsilonClosure(const std::set<State>& states) const
{
	std::set<State> closure = states;
	std::queue<State> queue;
	for (const auto& s : states)
	{
		queue.push(s);
	}

	while (!queue.empty())
	{
		State state = queue.front();
		queue.pop();

		auto transitions = GetTransitions(state, EPSILON);
		if (transitions.empty())
		{
			continue;
		}

		std::optional<Output> epsilonOutput;
		for (const auto& trans : transitions)
		{
			if (!epsilonOutput.has_value())
			{
				epsilonOutput = trans.output;
			}
			else if (epsilonOutput.value() != trans.output)
			{
				throw std::runtime_error("Non-determinizable: Output mismatch for epsilon transitions from state " + state);
			}

			if (!closure.contains(trans.nextState))
			{
				closure.insert(trans.nextState);
				queue.push(trans.nextState);
			}
		}
	}
	return closure;
}
