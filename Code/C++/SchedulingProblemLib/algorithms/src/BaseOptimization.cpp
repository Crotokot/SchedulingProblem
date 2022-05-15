#include "algorithms/include/BaseOptimization.h"

namespace scheduling_problem
{
	BaseOptimization BASELINE; // default baseline for other optimizers

	BaseOptimization::BaseOptimization(unsigned randomState) : randomState_(randomState), duration_(0)
	{
		rng_ = std::mt19937(randomState_);
	}

	Schedule BaseOptimization::schedule(const Graph& graph)
	{
		VertexTiesMap vertexTies = initializeMemory(graph);
		std::pair<IdsSetsMap, Schedule> state = makeState(graph, vertexTies);
		auto vertexIdsS = state.first;
		auto solution = state.second;
		auto itersCount = boost::num_vertices(graph);

		auto start = makeTimePoint();
		for (std::size_t iter = 0; iter < itersCount; iter++) {
			auto vertexID = choice(vertexIdsS["availableIds"]);
			updateState(graph, vertexIdsS, solution, vertexTies, vertexID);
		}
		auto stop = makeTimePoint();
		calculateDuration({ start, stop });
		return solution;
	}
	
	VertexTiesMap BaseOptimization::initializeMemory(const Graph& graph)
	{
		std::size_t nVertex = boost::num_vertices(graph);
		VertexID_Map vertexIndexes = boost::get(boost::vertex_index_t(), graph);
		std::vector<std::size_t> successors(nVertex);
		std::vector<std::size_t> predecessors(nVertex);
		return VertexTiesMap({ std::make_pair("successors", successors), 
			std::make_pair("predecessors", predecessors) });
	}

	std::pair<IdsSetsMap, Schedule> 
		BaseOptimization::makeState(const Graph& graph, VertexTiesMap& vertexTies)
	{
		boost::graph_traits<Graph>::vertex_iterator vertexBegin, vertexEnd;
		boost::tie(vertexBegin, vertexEnd) = boost::vertices(graph);
		std::size_t nVertex = boost::num_vertices(graph);
		Schedule solution = Schedule(nVertex);
		std::unordered_set<std::size_t> availableIds = std::unordered_set<std::size_t>(), 
			memorizedIds = std::unordered_set<std::size_t>();
		
		boost::graph_traits<Graph>::out_edge_iterator outEdgeBegin, outEdgeEnd;
		boost::graph_traits<Graph>::in_edge_iterator inEdgeBegin, inEdgeEnd;
		for (; vertexBegin < vertexEnd; vertexBegin++) {
			boost::tie(inEdgeBegin, inEdgeEnd) = boost::in_edges(*vertexBegin, graph);
			vertexTies["predecessors"][*vertexBegin] = inEdgeEnd - inEdgeBegin;
			boost::tie(outEdgeBegin, outEdgeEnd) = boost::out_edges(*vertexBegin, graph);
			vertexTies["successors"][*vertexBegin] = outEdgeEnd - outEdgeBegin;
			
			if (vertexTies["predecessors"][*vertexBegin] == 0) {
				availableIds.insert(*vertexBegin);
			}
		}

		return std::pair<IdsSetsMap, Schedule>(IdsSetsMap({ std::make_pair("availableIds", availableIds),
			std::make_pair("memorizedIds", memorizedIds) }), solution);
	}

	void BaseOptimization::updateState(const Graph& graph, IdsSetsMap& vertexIdS, 
		Schedule& solution, VertexTiesMap& vertexTies, std::size_t vertexID)
	{
		vertexIdS["availableIds"].erase(vertexID);
		vertexIdS["memorizedIds"].insert(vertexID);
		weight_t currentSolutionCost = solutionCost(graph, vertexIdS["memorizedIds"]);
		solution.append(vertexID, currentSolutionCost);

		boost::graph_traits<Graph>::out_edge_iterator outEdgeBegin, outEdgeEnd;
		boost::tie(outEdgeBegin, outEdgeEnd) = boost::out_edges(vertexID, graph);
		for (; outEdgeBegin < outEdgeEnd; outEdgeBegin++) {
			std::size_t targetVID = boost::target(*outEdgeBegin, graph);
			vertexTies["predecessors"][targetVID] -= 1;
			if (vertexTies["predecessors"][targetVID] == 0) {
				vertexIdS["availableIds"].insert(targetVID);
			}
		}
		boost::graph_traits<Graph>::in_edge_iterator inEdgeBegin, inEdgeEnd;
		boost::tie(inEdgeBegin, inEdgeEnd) = boost::in_edges(vertexID, graph);
		for (; inEdgeBegin < inEdgeEnd; inEdgeBegin++) {
			std::size_t sourceVID = boost::source(*inEdgeBegin, graph);
			vertexTies["successors"][sourceVID] -= 1;
			if (vertexTies["successors"][sourceVID] == 0) {
				vertexIdS["memorizedIds"].erase(sourceVID);
			}
		}
	}

	std::size_t BaseOptimization::choice(const std::unordered_set<std::size_t>& availableIds)
	{
		std::uniform_int_distribution<std::size_t> d(0, availableIds.size() - 1);
		auto shift = d(rng_);
		auto iter = availableIds.begin();
		for (; shift > 0; iter++, shift--);
		return *iter;
	}

	weight_t BaseOptimization::solutionCost(const Graph& graph, 
		const std::unordered_set<std::size_t>& memorizedIds)
	{
		weight_t cost = 0;
		std::for_each(memorizedIds.begin(), memorizedIds.end(), 
			[&](auto vertexID) 
			{
				cost += boost::get(vertex_weight_t(), graph, vertexID);
			});
		return cost;
	}

	std::chrono::steady_clock::time_point BaseOptimization::makeTimePoint() const
	{
		return std::chrono::steady_clock::now();
	}

	void BaseOptimization::calculateDuration(const std::pair<std::chrono::steady_clock::time_point, 
		std::chrono::steady_clock::time_point>& segment)
	{
		duration_ = std::chrono::duration_cast<std::chrono::microseconds>(segment.second - segment.first).count();
	}

	long long BaseOptimization::duration() const
	{
		return duration_;
	}

	void BaseOptimization::setParams(const std::unordered_map<std::string, double>& params)
	{

	}
}