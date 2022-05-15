#include "algorithms/include/ScheduleCorrector.h"

namespace scheduling_problem
{
	//ScheduleCorrector::ScheduleCorrector(std::mt19937 rng) : rng_(rng) {}

	ScheduleCorrector::ScheduleCorrector(unsigned randomState) : BaseOptimization(randomState) {}
	//{
	//	rng_ = std::mt19937(randomState);
	//}

	std::unordered_map<std::string, std::vector<std::size_t>>
		ScheduleCorrector::makeBounds(const Graph& graph, const Schedule& solution) const
	{
		std::size_t nVertex = boost::num_vertices(graph);
		std::vector<std::size_t> positions(nVertex), lowerBounds(nVertex), 
			upperBounds(nVertex), lastSuccessor(nVertex);
		std::size_t pos = 0;
		for (auto& pair : solution) {
			positions[pair.first] = pos++;
		}
		for (auto& pair : solution) {
			auto vertexID = pair.first;
			auto successors = boost::adjacent_vertices(vertexID, graph);
			auto predecessors = boost::inv_adjacent_vertices(vertexID, graph);
			std::vector<std::size_t> successorsPos(successors.second - successors.first);
			std::transform(successors.first, successors.second, successorsPos.begin(),
				[&graph, &positions](auto target) -> std::size_t
				{
					return positions[target];
				}
			);
			auto minmax = std::minmax_element(successorsPos.begin(), successorsPos.end());
			upperBounds[vertexID] = successorsPos.size() ? *minmax.first - 1 : nVertex - 1;
			lastSuccessor[vertexID] = successorsPos.size() ? *minmax.second : nVertex;

			std::vector<std::size_t> predecessorsPos(predecessors.second - predecessors.first);
			std::transform(predecessors.first, predecessors.second, predecessorsPos.begin(),
				[&graph, &positions](auto source) -> std::size_t
				{
					return positions[source];
				}
			);
			lowerBounds[vertexID] = 
				predecessorsPos.size() ? *std::max_element(predecessorsPos.begin(), predecessorsPos.end()) + 1 : 0;
		}
		return std::unordered_map<std::string, std::vector<std::size_t>>(
			{
				std::make_pair("positions", positions),
				std::make_pair("lowerBounds", lowerBounds),
				std::make_pair("upperBounds", upperBounds),
				std::make_pair("lastSuccessor", lastSuccessor)
			}
		);
	}

	Schedule ScheduleCorrector::correct(const Graph &graph, const Schedule& solution,
		std::unordered_map<std::string, std::vector<std::size_t>>& bounds)
	{
		auto start = makeTimePoint();
		Schedule newSolution = Schedule(solution);
		bounds = makeBounds(graph, newSolution);
		auto vertexID = choice(bounds);
		if (vertexID < solution.size()) {
			auto tposition = selectPosition(vertexID, bounds);
			transition(graph, newSolution, vertexID, tposition, bounds);
		}
		auto stop = makeTimePoint();
		calculateDuration({ start, stop });
		return newSolution;
	}

	std::size_t ScheduleCorrector::choice(
		std::unordered_map<std::string, std::vector<std::size_t>>& bounds)
	{
		std::vector<std::size_t> moveables;
		for (std::size_t vertexID = 0; vertexID < bounds["lowerBounds"].size(); vertexID++) {
			if (bounds["upperBounds"][vertexID] - bounds["lowerBounds"][vertexID] > 0) {
				moveables.push_back(vertexID);
			}
		}

		auto index = std::uniform_int_distribution<std::size_t>(0, moveables.size() - 1)(rng_);

		return index < moveables.size() ? moveables[index] : index;
	}

	std::size_t ScheduleCorrector::selectPosition(std::size_t vertexID,
		std::unordered_map<std::string, std::vector<std::size_t>>& bounds)
	{
		std::size_t fromPos = bounds["lowerBounds"][vertexID],
					toPos = bounds["upperBounds"][vertexID], 
					currentPos = bounds["positions"][vertexID];
		std::vector<std::size_t> posiblePositions;
		for (std::size_t pos = fromPos; pos <= toPos; pos++) {
			if (pos != currentPos) {
				posiblePositions.push_back(pos);
			}
		}
		auto index = std::uniform_int_distribution<std::size_t>(0, posiblePositions.size() - 1)(rng_);
		return posiblePositions[index];
	}

	void ScheduleCorrector::transition(const Graph& graph, Schedule& solution, const std::size_t tvertex,
		const std::size_t tpos, std::unordered_map<std::string, std::vector<std::size_t>>& bounds)
	{
		std::size_t pos = bounds["positions"][tvertex];

		// more efficient but still has bugs
		{
			//short shift = pos > tpos ? 1 : -1;
			//std::size_t low = std::min({ pos, tpos }), 
			//	high = std::max({ pos, tpos });
			//for (auto& position : bounds["positions"]) {
			//	if (position >= low && position <= high) {
			//		position += shift;
			//	}
			//}
			//bounds["positions"][tvertex] = tpos;
			//std::size_t changedNum = high - low + 2;
			//high += std::min({ 0, -shift });
			//if (tpos > 0) {
			//	low += std::min({ 0, -shift });
			//}
			//if (tpos > 0) {
			//}

			//std::vector<weight_t> completedResources(changedNum);
			//auto selectPositions = [](auto& vertexIter, auto& bounds, auto& destination)
			//{
			//	std::transform(vertexIter.first, vertexIter.second, std::back_inserter(destination),
			//		[&bounds](auto vid) { return bounds["positions"][vid]; });
			//};

			//// recompute lowerBounds, upperBounds and lastSuccessor positions
			//for (auto& pair : solution) {
			//	auto vertexID = pair.first;
			//	auto successors = boost::adjacent_vertices(vertexID, graph);
			//	std::vector<std::size_t> successorsPos, predecessorsPos;

			//	if (bounds["upperBounds"][vertexID] == pos - 1 ||
			//		std::find(successors.first, successors.second, tvertex) != successors.second)
			//	{
			//		selectPositions(successors, bounds, successorsPos);
			//		bounds["upperBounds"][vertexID] =
			//			successorsPos.size() > 0 ? *std::min_element(successorsPos.begin(),
			//				successorsPos.end()) - 1 : solution.currentSize() - 1;
			//	}
			//	else if (bounds["upperBounds"][vertexID] >= low && bounds["upperBounds"][vertexID] < high) {
			//		bounds["upperBounds"][vertexID] += shift;
			//	}

			//	if (bounds["lastSuccessor"][vertexID] == pos ||
			//		std::find(successors.first, successors.second, tvertex) != successors.second)
			//	{
			//		if (!successorsPos.size()) {
			//			selectPositions(successors, bounds, successorsPos);
			//		}
			//		bounds["lastSuccessor"][vertexID] =
			//			successorsPos.size() > 0 ? *std::max_element(successorsPos.begin(),
			//				successorsPos.end()) : bounds["positions"][vertexID];
			//	}
			//	else if (std::min({ low + 1, tpos }) <= bounds["lastSuccessor"][vertexID] && bounds["lastSuccessor"][vertexID] <= high) {
			//		bounds["lastSuccessor"][vertexID] += shift;
			//	}

			//	if (low - 1 <= bounds["lastSuccessor"][vertexID] && bounds["lastSuccessor"][vertexID] <= high) {
			//		auto shiftedIndex = bounds["lastSuccessor"][vertexID] - low + 1;
			//		completedResources[shiftedIndex] += boost::get(vertex_weight_t(), graph, vertexID);
			//	}

			//	auto predecessors = boost::inv_adjacent_vertices(vertexID, graph);
			//	if (bounds["lowerBounds"][vertexID] == pos + 1 ||
			//		std::find(predecessors.first, predecessors.second, tvertex) != predecessors.second)
			//	{
			//		selectPositions(predecessors, bounds, predecessorsPos);
			//		bounds["lowerBounds"][vertexID] = 
			//			predecessorsPos.size() > 0 ? *std::max_element(predecessorsPos.begin(), predecessorsPos.end()) + 1 : 0;
			//	}
			//	else if (bounds["lowerBounds"][vertexID] > std::min({ low + 1, tpos }) && bounds["lowerBounds"][vertexID] <= high + 1) {
			//		bounds["lowerBounds"][vertexID] += shift;
			//	}
			//}

			/*std::vector<weight_t> costs(changedNum);
			low = std::min({ pos, tpos });
			high = std::max({ pos, tpos });
			costs[0] = low > 0 ? solution[low - 1].second : 0;
			low++;*/
		}

		auto replacedPair = solution[pos];
		solution.erase(solution.begin() + pos);
		solution.insert(solution.begin() + tpos, replacedPair);
		
		// more efficient but still has bugs
		{
			/*for (auto pairPos(low); pairPos <= high; pairPos++) {
				auto shiftedIndex = pairPos - low + 1,
					nodeWeight = boost::get(vertex_weight_t(), graph, solution[pairPos].first);
				costs[shiftedIndex] = costs[shiftedIndex - 1] - completedResources[shiftedIndex - 1] + nodeWeight;
				solution[pairPos] = std::pair<std::size_t, weight_t>(solution[pairPos].first, costs[shiftedIndex]);
			}*/
		}


		VertexTiesMap vertexTies = initializeMemory(graph);
		std::pair<IdsSetsMap, Schedule> state = makeState(graph, vertexTies);
		auto vertexIdsS = state.first;
		auto sol = state.second;
		for (auto& pair : solution) {
			auto vertexID = pair.first;
			updateState(graph, vertexIdsS, sol, vertexTies, vertexID);
			pair.second = sol[sol.currentSize() - 1].second;
		}
		

		solution.recomputeCost();
	}
}