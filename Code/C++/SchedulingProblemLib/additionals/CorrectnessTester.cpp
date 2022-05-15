#include "CorrectnessTester.h"

namespace scheduling_problem
{

	bool CorrectnessTester::check(const Schedule& schedule, const Schedule& solution, VertexTiesMap vertexTies)
	{
		bool correct = true;
		if (solution[solution.currentSize() - 1].first != schedule[solution.currentSize() - 1].first || 
			solution[solution.currentSize() - 1].second != schedule[solution.currentSize() - 1].second) {
			correct = false;
		}
		for (auto i(0); i < schedule.currentSize(); i++) {
			if (vertexTies["predecessors"][i] < 0 || vertexTies["successors"][i] < 0) {
				correct = false;
			}
		}
		return correct;
	}

	bool CorrectnessTester::checkPrecedence(const Graph& graph, const Schedule& schedule)
	{
		bool correct = true;
		std::unordered_set<std::size_t> completed;
		for (auto& pair : schedule) {

			auto predecessors = boost::inv_adjacent_vertices(pair.first, graph);
			std::for_each(predecessors.first, predecessors.second, [&](auto vid) { correct &= (completed.find(vid) != completed.end()); });
			if (!correct)
				break;
			completed.insert(pair.first);
		}
		return correct;
	}

    bool CorrectnessTester::isCorrect(const scheduling_problem::Graph& graph,
		const scheduling_problem::Schedule& schedule)
    {
        bool correct = checkPrecedence(graph, schedule);
		VertexTiesMap vertexTies = initializeMemory(graph);
		std::pair<IdsSetsMap, Schedule> state = makeState(graph, vertexTies);
		auto vertexIdsS = state.first;
		auto solution = state.second;
		try {
			for (auto& pair : schedule) {
				auto vertexID = pair.first;
				updateState(graph, vertexIdsS, solution, vertexTies, vertexID);
				correct &= check(schedule, solution, vertexTies);
				if (!correct) {
					throw std::exception();
				}
			}

		}
		catch (const std::exception&) {
			correct = false;
		}
        return correct;
    }
}