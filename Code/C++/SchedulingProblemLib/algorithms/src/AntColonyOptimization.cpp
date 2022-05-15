#include "algorithms/include/AntColonyOptimization.h"

namespace scheduling_problem
{
	AntColonyOptimization::AntColonyOptimization(double pheDecay, 
												 double threshold, 
												 double pheInfluence, 
												 double heuInfluence, 
												 unsigned epochs, 
												 unsigned ants, 
												 BaseOptimization& baseline, 
												 unsigned saturation, 
												 unsigned randomState) 
												 : GreedyOptimization(randomState)
												 , IterativeOptimization(baseline, saturation)
												 , pheDecay_(pheDecay)
												 , threshold_(threshold)
												 , pheInfluence_(pheInfluence)
												 , heuInfluence_(heuInfluence)
												 , epochs_(epochs)
												 , ants_(ants) 
	{

	}

	Schedule AntColonyOptimization::schedule(const Graph& graph)
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
					globalPheromoneUpdate(bestSolution, true);
					if (saturation_) {
						stagnationsCount = 0;
					}
				}
				else {
					globalPheromoneUpdate(solution);
					//globalPheromoneUpdate(bestSolution);
				}
			}
			descentDynamics_.push_back(bestSolution.cost());
			costDynamics_.push_back(epochResults[0].cost());
			itersCount_++;
			stagnationsCount++;
		}
		auto stop = makeTimePoint();
		calculateDuration({ start, stop });
		return bestSolution;
	}

	void AntColonyOptimization::initWeightMatrix(std::size_t nVertex, double initTrail)
	{
		weightMatrix_ = std::vector<std::vector<double>>(nVertex);
		std::for_each(weightMatrix_.begin(), weightMatrix_.end(),
			[&nVertex, &initTrail](auto& row) 
			{
				row = std::vector<double>(nVertex);
				std::fill(row.begin(), row.end(), initTrail);
			}
		);
	}

	std::vector<Schedule> AntColonyOptimization::
		completeEpoch(const Graph& graph, VertexTiesMap& vertexTies, std::size_t nSteps, double initTrail)
	{
		std::vector<Schedule> epochResults;
		for (unsigned ant = 0; ant < ants_; ant++) {
			auto state = makeState(graph, vertexTies);
			IdsSetsMap vertexIdS = state.first;
			Schedule solution = state.second;
			for (std::size_t step = 0; step < nSteps; step++) {
				std::size_t vertexID = choice(graph, vertexIdS, vertexTies, step);
				updateState(graph, vertexIdS, solution, vertexTies, vertexID);
				localPheromoneUpdate(vertexID, step, initTrail);
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

	std::size_t AntColonyOptimization::
		choice(const Graph& graph, IdsSetsMap& vertexIdS, VertexTiesMap& vertexTies, std::size_t step)
	{
		auto heuInfo = heuristicInfo(graph, vertexIdS, vertexTies);
		double q = std::uniform_real_distribution<double>(0, 1)(rng_);
		std::unordered_map<std::size_t, double> prod;
		std::for_each(heuInfo.begin(), heuInfo.end(),
			[&](auto& heuInfoPair) 
			{
				prod[heuInfoPair.first] = std::pow(weightMatrix_[step][heuInfoPair.first], pheInfluence_) *
					std::pow(heuInfoPair.second, heuInfluence_);
			}
		);
		std::size_t vertexID;
		if (q <= threshold_) {
			vertexID = std::max_element(prod.begin(), prod.end(),
				[](auto& elem_1, auto& elem_2)
				{
					return elem_1.second < elem_2.second; 
				}
			)->first;
		}
		else {
			double prodSum = std::accumulate(prod.begin(), prod.end(), 0.0,
				[](auto& init, auto& elem)
				{
					return init + elem.second;
				}
			);
			std::for_each(prod.begin(), prod.end(),
				[&prodSum](auto& pair) 
				{
					pair.second =  pair.second / prodSum;
				}
			);
			vertexID = hypergeometricVal(prod);
		}
		return vertexID;
	}

	Schedule AntColonyOptimization::
		selectBestEpochSolution(std::vector<Schedule>& epochResults)
	{
		std::sort(epochResults.begin(), epochResults.end(),
			[](auto& sh_1, auto& sh_2)
			{
				return sh_1.cost() < sh_2.cost();
			}
		);
		auto solution = epochResults[0];
		return solution;
	}

	void AntColonyOptimization::localPheromoneUpdate(std::size_t vertexID, std::size_t step, double initTrail)
	{
		weightMatrix_[step][vertexID] = weightMatrix_[step][vertexID] * (1 - pheDecay_) + pheDecay_ * initTrail;
	}

	void AntColonyOptimization::globalPheromoneUpdate(const Schedule& solution, bool isBest)
	{
		auto delta = isBest ? pheDecay_ / solution.cost() : 0;
		auto alpha = (1 - pheDecay_);
		for (std::size_t pos = 0; pos < solution.currentSize(); pos++) {
			weightMatrix_[pos][solution[pos].first] = alpha * weightMatrix_[pos][solution[pos].first] + delta;
		}
	}

	std::size_t AntColonyOptimization::
		hypergeometricVal(std::unordered_map<std::size_t, double>& probs)
	{
		if (probs.size() > 1) {
			double cumsum = 0;
			std::for_each(probs.begin(), probs.end(),
				[&cumsum](auto& pair)
				{
					pair.second += cumsum;
					cumsum = pair.second;
				}
			);

			double uniform_sample = std::uniform_real_distribution<double>(0, cumsum)(rng_);
			auto iter = probs.begin();
			while (uniform_sample > iter->second) iter++;
			std::size_t vertexID = iter->first;
			return vertexID;
		}
		else {
			return probs.begin()->first;
		}
	}

	void AntColonyOptimization::setParams(double pheDecay, double threshold, double pheInfluence, double heuInfluence)
	{
		pheDecay_ = pheDecay;
		threshold_ = threshold;
		pheInfluence_ = pheInfluence;
		heuInfluence_ = heuInfluence;
	}

	void AntColonyOptimization::setParams(std::unordered_map<std::string, double> params)
	{
		for (auto param : params) {
			switch (paramsMap_[param.first]) {
				case Params::PHEDECAY:
					pheDecay_ = param.second;
					break;
				case Params::THRESHOLD:
					threshold_ = param.second;
					break;
				case Params::PHEINFLUENCE:
					pheInfluence_ = param.second;
					break;
				case Params::HEUINFLUENCE:
					heuInfluence_ = param.second;
					break;
				case Params::EPOCHS:
					epochs_ = (unsigned)param.second;
					break;
				case Params::ANTS:
					ants_ = (unsigned)param.second;
					break;
				default:
					IterativeOptimization::setParams({ param });
					break;
			}
		}
	}
}