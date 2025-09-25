#include "MooreMachine.h"
#include "MealyMachine.h"
#include <algorithm>
#include <fstream>
#include <regex>

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

MooreMachine::MooreMachine(State initialState)
	: m_currentState(std::move(initialState))
{
}

MooreMachine::MooreMachine(MealyMachine& mealyMachine)
{
	ConvertFromMealy(mealyMachine);
}

void MooreMachine::AddStateOutput(const State& state, const Output& output)
{
	m_stateOutputs[state] = output;

	if (std::find(m_states.begin(), m_states.end(), state) == m_states.end())
	{
		m_states.push_back(state);
	}
	if (std::find(m_outputs.begin(), m_outputs.end(), output) == m_outputs.end())
	{
		m_outputs.push_back(output);
	}
}

void MooreMachine::AddTransition(const State& from, const Input& input, const State& to)
{
	m_transitions[from].emplace(input, Transition(to));

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
}

void MooreMachine::FromDot(const std::string& fileName)
{
	std::ifstream file(fileName);
	AssertInputIsOpen(file, fileName);

	const std::regex edgeRegex(R"((\w+)\s*->\s*(\w+)\s*\[label\s*=\s*\"([^\"]+)\"\]\s*;)");
	const std::regex outputRegex(R"((\w+)\s*\[label\s*=\s*\"(\w+)\\noutput:\s*([^\"]+)\"\]\s*;)");
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

		if (line.find("[shape=circle]") != std::string::npos ||
			line.find("[shape=doublecircle]") != std::string::npos)
		{
			auto pos = line.find('[');
			if (pos != std::string::npos)
			{
				State state = line.substr(0, pos);
				state.erase(state.find_last_not_of(" \t") + 1);

				if (m_stateOutputs.find(state) == m_stateOutputs.end())
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

void MooreMachine::ConvertFromMealy(MealyMachine& mealy)
{
	Clear();

	auto mealyStates = mealy.GetStates();
	auto mealyInputs = mealy.GetInputs();

	for (const auto& state : mealyStates)
	{
		std::string output = "0";

		for (const auto& input : mealyInputs)
		{
			if (!mealy.HasTransition(state, input))
			{
				continue;
			}
			output = mealy.GetTransitionOutput(state, input);
			break;
		}

		AddStateOutput(state, output);
	}

	for (const auto& fromState : mealyStates)
	{
		for (const auto& input : mealyInputs)
		{
			if (!mealy.HasTransition(fromState, input))
			{
				continue;
			}
			std::string toState = mealy.GetNextState(fromState, input);
			AddTransition(fromState, input, toState);
		}
	}

	if (!mealyStates.empty())
	{
		m_currentState = mealyStates[0];
	}
}

bool MooreMachine::HasTransition(const Machine::State& from, const Machine::Input& input)
{
	auto stateIt = m_transitions.find(from);
	if (stateIt == m_transitions.end())
	{
		return false;
	}
	return stateIt->second.find(input) != stateIt->second.end();
}

Machine::State MooreMachine::GetNextState(const Machine::State& fromState, const Machine::Input& input) const
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

	return inputIt->second.nextState;
}

Machine::Output MooreMachine::GetOutputForState(const Machine::State& state) const
{
	auto it = m_stateOutputs.find(state);
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
