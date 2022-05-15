#pragma once

#include "BaseOptimization.h"
#include <vector>

namespace scheduling_problem
{	
	extern BaseOptimization BASELINE;

	class BranchBoundOptimization : public BaseOptimization
	{
	private:
		BaseOptimization& baseline_;

	public:

		BranchBoundOptimization(BaseOptimization& baseline = BASELINE, 
								unsigned randomState = 42);

		Schedule schedule(const Graph& graph);

	private:
		void schedule(const Graph& graph, VertexTiesMap vertexTies, IdsSetsMap vertexIdsS, 
			Schedule solution, Schedule& bestSolution, std::size_t vertexID);
	};
}


