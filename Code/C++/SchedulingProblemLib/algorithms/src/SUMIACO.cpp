#include "algorithms/include/SUMIACO.h"

namespace scheduling_problem
{
	SUMIACO::SUMIACO(double pheDecay, 
					 double threshold, 
					 double pheInfluence, 
					 double heuInfluence, 
					 unsigned epochs, 
					 unsigned ants, 
					 BaseOptimization& baseline, 
					 unsigned saturation, 
					 unsigned randomState, 
					 unsigned localSearchIters) 
					 : AntColonyOptimization(pheDecay, 
					 						 threshold, 
					 						 pheInfluence, 
					 						 heuInfluence, 
					 						 epochs, 
					 						 ants, 
					 						 baseline, 
					 						 saturation, 
					 						 randomState)
					 , ScheduleCorrector(randomState)
					 , localSearchIters_(localSearchIters)
	{

	}
	
	void SUMIACO::localSearch(const Graph& graph, Schedule& localBest)
	{
		Schedule solution = Schedule(localBest);
		auto bounds = makeBounds(graph, solution);
		for (unsigned iterNum(0); iterNum < localSearchIters_; iterNum++) {
			solution = correct(graph, solution, bounds);
			if (solution.cost() < localBest.cost()) {
				localBest = solution;
			}
		}
	}

	void SUMIACO::localSearchEpoch(const Graph& graph, std::vector<Schedule>& epochResults)
	{
		for (auto& solution : epochResults) {
			localSearch(graph, solution);
		}
	}

	Schedule SUMIACO::schedule(const Graph& graph)
	{
		auto start = makeTimePoint();
		std::size_t nVertex = boost::num_vertices(graph);
		Schedule bestSolution = baseline_.schedule(graph);
		double initTrail = 1. / bestSolution.cost();
		initWeightMatrix(nVertex, initTrail);
		VertexTiesMap vertexTies = initializeMemory(graph);
		unsigned stagnationsCount = 0;
		while ((saturation_ && stagnationsCount < saturation_) || (!saturation_ && itersCount_ < epochs_)) {
			std::vector<Schedule> epochResults = completeEpoch(graph, vertexTies, nVertex, initTrail);
			localSearchEpoch(graph, epochResults);
			Schedule solution = selectBestEpochSolution(epochResults);
			if (solution.cost() < bestSolution.cost()) {
				bestSolution = solution;
				globalPheromoneUpdate(bestSolution, true);
				if (saturation_) {
					stagnationsCount = 0;
				}
			}
			else {
				globalPheromoneUpdate(solution);
			}
			itersCount_++;
			stagnationsCount++;
		}
		auto stop = makeTimePoint();
		calculateDuration({ start, stop });
		return bestSolution;
	}

	void SUMIACO::setParams(std::unordered_map<std::string, double> params)
	{
		for (auto param : params) {
			switch (paramsMap_[param.first]) {
				case Params::LOCALSEARCHITERS:
					localSearchIters_ = (unsigned)param.second;
					break;
				default:
					AntColonyOptimization::setParams({ param });
					break;
			}
		}
	}
}