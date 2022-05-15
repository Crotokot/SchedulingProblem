#include "algorithms/include/SimulatedAnnealing.h"

namespace scheduling_problem
{
	SimulatedAnnealing::SimulatedAnnealing(double minTemp, 
										   double maxTemp, 
										   ReductionRules reductionRule, 
										   BaseOptimization& baseline, 
										   unsigned saturation, 
										   unsigned randomState) 
										   : ScheduleCorrector(randomState)
										   , IterativeOptimization(baseline, saturation)
										   , minTemperature_(minTemp)
										   , maxTemperature_(maxTemp)
										   , reductionRule_(reductionRule)
	{
		switch (reductionRule) 
		{
		case ReductionRules::boltzmann:
			reduceTemperature_ = boltzmannRule;
			break;
		case ReductionRules::couchy:
			reduceTemperature_ = couchyRule;
			break;
		case ReductionRules::mixed:
			reduceTemperature_ = mixedRule;
			break;
		default:
			throw;
		}
	}

	bool SimulatedAnnealing::isTransitionAcceptance(double energyDelta, double temperature)
	{
		std::uniform_real_distribution<double> dist(0, 1);
		double prob = dist(rng_);
		double transitionProb = energyDelta > 0 ? exp(-(double)energyDelta / temperature) : 1.1;
		return prob <= transitionProb;
	}

	Schedule SimulatedAnnealing::schedule(const Graph& graph)
	{
		auto start = makeTimePoint();
		Schedule bestSolution = baseline_.schedule(graph);
		Schedule solution = bestSolution;
		auto bounds = makeBounds(graph, solution);
		unsigned stagnationsCount = 0;
		double currentTemperature;
		itersCount_ = 0;
		while ((currentTemperature = reduceTemperature_(maxTemperature_, itersCount_ + 1)) > minTemperature_)
		{
			Schedule newSolution = correct(graph, solution, bounds);
			auto energyDelta = (double)newSolution.cost() - solution.cost();
			if (isTransitionAcceptance(energyDelta, currentTemperature)) {
				solution = newSolution;
			}
			if (solution.cost() < bestSolution.cost()) {
				bestSolution = solution;
				stagnationsCount = 0;
			}
			itersCount_++;
			stagnationsCount++;
			if (saturation_ && stagnationsCount > saturation_) {
				break;
			}
			costDynamics_.push_back(newSolution.cost());
			descentDynamics_.push_back(bestSolution.cost());
		}
		auto stop = makeTimePoint();
		calculateDuration({ start, stop });
		return bestSolution;
	}

	unsigned SimulatedAnnealing::itersCount()
	{
		return itersCount_;
	}

	double SimulatedAnnealing::boltzmannRule(double initTemperature, std::size_t iterNum)
	{
		return initTemperature / std::log2(1 + iterNum);
	}

	double SimulatedAnnealing::couchyRule(double initTemperature, std::size_t iterNum)
	{
		return initTemperature / (1 + iterNum);
	}

	double SimulatedAnnealing::mixedRule(double initTemperature, std::size_t iterNum)
	{
		return initTemperature * std::log2(1 + iterNum) / (1 + iterNum);
	}

	void SimulatedAnnealing::setParams(std::unordered_map<std::string, double> params)
	{
		for (auto param : params) {
			switch (paramsMap_[param.first]) {
			case Params::MINTEMP:
				minTemperature_ = param.second;
				break;
			case Params::MAXTEMP:
				maxTemperature_ = param.second;
				break;
			default:
				IterativeOptimization::setParams({ param });
				break;
			}
		}
	}
}