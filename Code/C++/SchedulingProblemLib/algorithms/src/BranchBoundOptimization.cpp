#include "algorithms/include/BranchBoundOptimization.h"
#include <iostream>

namespace scheduling_problem
{
	BranchBoundOptimization::BranchBoundOptimization(BaseOptimization& baseline, 
													 unsigned randomState) 
													 : BaseOptimization(randomState)
													 , baseline_(baseline)
	{
		
	}

	Schedule BranchBoundOptimization::schedule(const Graph& graph)
	{
		auto start = makeTimePoint();
		VertexTiesMap vertexTies = initializeMemory(graph);
		std::pair<IdsSetsMap, Schedule> state = makeState(graph, vertexTies);
		auto vertexIdsS = state.first;
		auto solution = state.second;
		auto itersCount = boost::num_vertices(graph);
		auto bestSolution = baseline_.schedule(graph);
		for (auto vertexID : vertexIdsS["availableIds"]) {
			schedule(graph, vertexTies, vertexIdsS, solution, bestSolution, vertexID);
		}
		auto stop = makeTimePoint();
		calculateDuration({ start, stop });
		return bestSolution;
	}

	void BranchBoundOptimization::schedule(const Graph& graph, VertexTiesMap vertexTies, 
		IdsSetsMap vertexIdsS, Schedule solution, Schedule& bestSolution, std::size_t vertexID)
	{
		updateState(graph, vertexIdsS, solution, vertexTies, vertexID);
		if (solution.currentSize() < bestSolution.currentSize()) {
			if (solution.cost() < bestSolution.cost()) {
				for (auto nextVertexID : vertexIdsS["availableIds"]) {
					auto ties = initializeMemory(graph);
					boost::graph_traits<Graph>::vertex_iterator b, e;
					for (boost::tie(b, e) = boost::vertices(graph); b < e; b++) {
						ties["predecessors"][*b] = vertexTies["predecessors"][*b];
						ties["successors"][*b] = vertexTies["successors"][*b];
					}
					schedule(graph, ties, vertexIdsS, solution, bestSolution, nextVertexID);
				}
			}
		}
		else if (solution.cost() < bestSolution.cost()) {
				bestSolution = solution;
		}
	}
}
