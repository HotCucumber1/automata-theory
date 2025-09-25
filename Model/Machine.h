#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Machine
{
public:
	using State = std::string;
	using Input = std::string;
	using Output = std::string;

public:
	Machine() = default;
	virtual void FromDot(const std::string& fileName) = 0;
	virtual void SaveToDot(const std::string& fileName) = 0;
	virtual bool HasTransition(const State& from, const Input& input) = 0;

	const std::vector<Input>& GetInputs() const
	{
		return m_inputs;
	}

	const std::vector<Output>& GetOutputs() const
	{
		return m_outputs;
	}

	const std::vector<State>& GetStates() const
	{
		return m_states;
	}

protected:
	std::vector<State> m_states;
	std::vector<Input> m_inputs;
	std::vector<Output> m_outputs;
};
