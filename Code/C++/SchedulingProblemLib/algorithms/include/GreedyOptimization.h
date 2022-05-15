#pragma once

#include "algorithms/include/BaseOptimization.h"

namespace scheduling_problem 
{
	class GreedyOptimization : virtual public BaseOptimization
	{
	public:
		GreedyOptimization();

		virtual Schedule schedule(const Graph& graph);

	protected:
		GreedyOptimization(unsigned randomState);

		std::unordered_map<std::size_t, double>
			heuristicInfo(const Graph& graph, IdsSetsMap& vertexIdS, VertexTiesMap& vertexTies);

		std::size_t choice(const Graph& graph, IdsSetsMap& vertexIdS, VertexTiesMap& vertexTies);
	};
}