#include "Machine.h"

#include <map>
#include <unordered_map>

std::vector<std::vector<Machine::State>> Machine::BreakForPartitions(
	const std::vector<std::vector<State>>& initialPartitions
) const
{
	auto partitions = initialPartitions;
	while (true)
	{
		std::unordered_map<State, int> stateToGroupIndex;
		for (int i = 0; i < partitions.size(); ++i)
		{
			for (const auto& state : partitions[i])
			{
				stateToGroupIndex[state] = i;
			}
		}
		std::vector<std::vector<State>> newPartitions;

		bool hasChanged = false;
		for (const auto& group : partitions)
		{
			if (group.size() <= 1)
			{
				newPartitions.push_back(group);
				continue;
			}

			std::map<std::vector<int>, std::vector<State>> subGroups;
			for (const auto& state : group)
			{
				std::vector<int> destinationGroups;
				for (const auto& input : m_inputs)
				{
					State nextState = GetNextState(state, input);
					destinationGroups.push_back(stateToGroupIndex.at(nextState));
				}
				subGroups[destinationGroups].push_back(state);
			}

			if (subGroups.size() > 1)
			{
				hasChanged = true;
			}

			for (const auto& pair : subGroups)
			{
				newPartitions.push_back(pair.second);
			}
		}
		partitions = newPartitions;

		if (!hasChanged)
		{
			break;
		}
	}
	return partitions;
}