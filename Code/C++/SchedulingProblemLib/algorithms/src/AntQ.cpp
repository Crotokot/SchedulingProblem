#include "algorithms/include/AntQ.h"

namespace scheduling_problem
{
	AntQ::AntQ(double pheDecay, 
			   double threshold, 
			   double pheInfluence, 
			   double heuInfluence, 
			   double gamma, 
			   double W, 
			   unsigned epochs, 
			   unsigned ants, 
			   BaseOptimization& baseline, 
			   unsigned saturation, 
			   unsigned randomState) 
			   : AntColonyOptimization(pheDecay, 
			   						   threshold, 
			   						   pheInfluence, 
			   						   heuInfluence, 
			   						   epochs, 
			   						   ants, 
			   						   baseline, 
			   						   saturation, 
			   						   randomState)
			   , gamma_(gamma)
			   , W_(W)
	{

	}

	Schedule AntQ::schedule(const Graph& graph)
	{
		auto start = makeTimePoint();
		std::size_t nVertex = boost::num_vertices(graph);
		Schedule bestSolution = baseline_.schedule(graph);
		double initTrail = 1. / bestSolution.cost();
		initWeightMatrix(nVertex, initTrail);
		VertexTiesMap vertexTies = initializeMemory(graph);
		itersCount_ = 0;
		unsigned stagnationsCount = 0;
		while ((saturation_ && stagnationsCount < saturation_) || (!saturation_ && itersCount_ < epochs_)) {
			std::vector<Schedule> epochResults = completeEpoch(graph, vertexTies, nVertex, initTrail);
			for (auto& solution : epochResults) {
				if (solution.cost() < bestSolution.cost()) {
					bestSolution = solution;
					globalAQValueUpdate(graph, bestSolution, vertexTies, true);
					if (saturation_) {
						stagnationsCount = 0;
					}
				}
				else {
					globalAQValueUpdate(graph, solution, vertexTies);
				}
			}
			itersCount_++;
			stagnationsCount++;
		}
		auto stop = makeTimePoint();
		calculateDuration({ start, stop });
		return bestSolution;
	}

	void AntQ::setParams(std::unordered_map<std::string, double> params)
	{
		for (auto param : params) {
			switch (paramsMap_[param.first]) {
				case Params::GAMMA:
					gamma_ = param.second;
					break;
				case Params::W:
					W_ = param.second;
					break;
				default:
					AntColonyOptimization::setParams({ param });
					break;
			}
		}
	}

	std::vector<Schedule> AntQ::completeEpoch(const Graph& graph, 
		VertexTiesMap& vertexTies, std::size_t nSteps, double initTrail)
	{
		std::vector<Schedule> epochResults;
		for (unsigned ant = 0; ant < ants_; ant++) {
			auto state = makeState(graph, vertexTies);
			IdsSetsMap vertexIdS = state.first;
			Schedule solution = state.second;
			for (std::size_t step = 0; step < nSteps; step++) {
				std::size_t vertexID = choice(graph, vertexIdS, vertexTies, step);
				updateState(graph, vertexIdS, solution, vertexTies, vertexID);
				localAQValueUpdate(vertexID, step, vertexIdS["availableIds"]);
			}
			epochResults.push_back(solution);
		}
		std::sort(epochResults.begin(), epochResults.end(),
			[](auto& sol1, auto& sol2)
			{
				return sol1.cost() < sol2.cost();
			}
		);
		return epochResults;
	}

	void AntQ::localAQValueUpdate(std::size_t vertexID, std::size_t step, const std::unordered_set<std::size_t>& available)
	{
		
		weightMatrix_[step][vertexID] = (1 - pheDecay_) * weightMatrix_[step][vertexID];
		if (step < weightMatrix_.size() - 1) {
			weightMatrix_[step][vertexID] += pheDecay_ * gamma_ * getMaxAQ(step, available);
		}
	}

	void AntQ::globalAQValueUpdate(const Graph& graph, const Schedule& solution, VertexTiesMap& vertexTies, bool isBest)
	{
		auto state = makeState(graph, vertexTies);
		IdsSetsMap vertexIdS = state.first;
		Schedule schedule = state.second;
		double delta = isBest ? 1. / solution.cost() : 0.0;
		for (std::size_t pos = 0; pos < solution.size(); pos++) {
			auto vertexID = solution[pos].first;
			updateState(graph, vertexIdS, schedule, vertexTies, vertexID);
			weightMatrix_[pos][vertexID] = (1 - pheDecay_) * weightMatrix_[pos][vertexID] + pheDecay_ * delta;
			if (pos < weightMatrix_.size() - 1) {
				weightMatrix_[pos][vertexID] += pheDecay_ * gamma_ * getMaxAQ(pos, vertexIdS["availableIds"]);
			}
		}
	}

	double AntQ::getMaxAQ(std::size_t pos, const std::unordered_set<std::size_t>& available)
	{
		std::vector<double> AQ;
		std::transform(available.begin(), available.end(), std::back_inserter(AQ),
			[&](auto& vertex)
			{
				return weightMatrix_[pos + 1][vertex];
			}
		);
		return *std::max_element(AQ.begin(), AQ.end());
	}
}