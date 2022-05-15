#include "algorithms/include/GreedyOptimization.h"

namespace scheduling_problem
{
	GreedyOptimization::GreedyOptimization() : BaseOptimization() {}

	GreedyOptimization::GreedyOptimization(unsigned randomState) : BaseOptimization(randomState) {}

	Schedule GreedyOptimization::schedule(const Graph& graph)
	{
		auto start = makeTimePoint();
		VertexTiesMap vertexTies = initializeMemory(graph);
		std::pair<IdsSetsMap, Schedule> state = makeState(graph, vertexTies);
		auto vertexIdsS = state.first;
		auto solution = state.second;
		std::size_t itersCount = boost::num_vertices(graph);;
		for (std::size_t iter = 0; iter < itersCount; iter++) {
			auto vertexID = choice(graph, vertexIdsS, vertexTies);
			updateState(graph, vertexIdsS, solution, vertexTies, vertexID);
		}
		auto stop = makeTimePoint();
		calculateDuration({ start, stop });
		return solution;
	}

	std::unordered_map<std::size_t, double> 
		GreedyOptimization::heuristicInfo(const Graph& graph, 
			IdsSetsMap& vertexIdsS, VertexTiesMap& vertexTies)
	{
		std::unordered_map<std::size_t, double> heuInfo = std::unordered_map<std::size_t, double>();
		weight_t cost = solutionCost(graph, vertexIdsS["memorizedIds"]);
		std::for_each(vertexIdsS["availableIds"].begin(), vertexIdsS["availableIds"].end(),
			[&](auto& vertexID)
			{
				auto in_edges = boost::in_edges(vertexID, graph);
				weight_t completeResource = 0;
				for (auto beg = in_edges.first; beg < in_edges.second; beg++) {
					auto source = boost::source(*beg, graph);
					if (vertexTies["successors"][source] == 1) {
						completeResource += boost::get(vertex_weight_t(), graph, source);
					}
				}
				auto vertexResource = boost::get(vertex_weight_t(), graph, vertexID);
				heuInfo[vertexID] = vertexResource > 0 ? 1. / (cost - completeResource + vertexResource) : 1.;
			}
		);
		return heuInfo;
	}

	std::size_t GreedyOptimization::choice(const Graph& graph, 
		IdsSetsMap& vertexIdsS, VertexTiesMap& vertexTies)
	{
		auto heuInfo = heuristicInfo(graph, vertexIdsS, vertexTies);
		std::pair<std::size_t, double> vertexData = *heuInfo.begin();
		std::for_each(heuInfo.begin(), heuInfo.end(),
			[&](auto& data) 
			{
				if (data.second > vertexData.second) {
					vertexData = data;
				}
			}
		);
		return vertexData.first;
	}
}