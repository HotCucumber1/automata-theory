#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

class Machine
{
public:
	using State = std::string;
	using Input = std::string;
	using Output = std::string;

	virtual void FromDot(const std::string& fileName) = 0;
	virtual void SaveToDot(const std::string& fileName) = 0;
	virtual bool HasTransition(const State& from, const Input& input) = 0;
	virtual std::unique_ptr<Machine> GetMinimized() const = 0;
	virtual State GetNextState(const State& fromState, const Input& input) const = 0;
	virtual State GetInitialState() const = 0;

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
	virtual ~Machine() = default;

protected:
	std::vector<std::vector<State>> BreakForPartitions(
		const std::vector<std::vector<State>>& initialPartitions) const;

	std::vector<State> m_states;
	std::vector<Input> m_inputs;
	std::vector<Output> m_outputs;
};
