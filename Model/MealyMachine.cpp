#include "MealyMachine.h"
#include "MooreMachine.h"
#include <fstream>
#include <regex>
#include <utility>

void AssertInputIsOpen(const std::ifstream& file, const std::string& fileName)
{
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + fileName);
	}
}

void AssertOutputIsOpen(const std::ofstream& file, const std::string& fileName)
{
	if (!file.is_open())
	{
		throw std::runtime_error("Cannot open file: " + fileName);
	}
}

MealyMachine::MealyMachine(Machine::State initialState)
	: m_currentState(std::move(initialState))
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
	const Machine::State& from,
	const Machine::Input& input,
	const Machine::State& to,
	const Machine::Output& output)
{
	m_transitions[from].emplace(input, Transition(to, output));

	if (std::find(m_states.begin(), m_states.end(), from) == m_states.end())
	{
		m_states.push_back(from);
	}
	if (std::find(m_states.begin(), m_states.end(), to) == m_states.end())
	{
		m_states.push_back(to);
	}
	if (std::find(m_inputs.begin(), m_inputs.end(), input) == m_inputs.end())
	{
		m_inputs.push_back(input);
	}
	if (std::find(m_outputs.begin(), m_outputs.end(), output) == m_outputs.end())
	{
		m_outputs.push_back(output);
	}
}
bool MealyMachine::HasTransition(const Machine::State& from, const Machine::Input& input)
{
	auto stateIt = m_transitions.find(from);
	if (stateIt == m_transitions.end())
	{
		return false;
	}
	return stateIt->second.find(input) != stateIt->second.end();
}

void MealyMachine::ConvertFromMoore(MooreMachine& moore)
{
	Clear();

	auto mooreStates = moore.GetStates();
	auto mooreInputs = moore.GetInputs();

	for (const auto& state : mooreStates)
	{
		if (std::find(m_states.begin(), m_states.end(), state) == m_states.end())
		{
			m_states.push_back(state);
		}
	}

	for (const auto& fromState : mooreStates)
	{
		for (const auto& input : mooreInputs)
		{
			if (!moore.HasTransition(fromState, input))
			{
				continue;
			}
			std::string toState = moore.GetNextState(fromState, input);
			std::string output = moore.GetOutputForState(toState);
			AddTransition(fromState, input, toState, output);
		}
	}

	auto mooreOutputs = moore.GetOutputs();
	for (const auto& output : mooreOutputs)
	{
		m_outputs.push_back(output);
	}
}

std::string MealyMachine::GetTransitionOutput(const std::string& fromState, const std::string& input) const
{
	return GetTransition(fromState, input).output;
}

MealyMachine::Transition MealyMachine::GetTransition(const Machine::State& fromState, const Machine::Input& input) const
{
	auto stateIt = m_transitions.find(fromState);
	if (stateIt == m_transitions.end())
	{
		throw std::runtime_error("No transitions from state: " + fromState);
	}

	auto inputIt = stateIt->second.find(input);
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
