#pragma once

#include "algorithms/include/ScheduleCorrector.h"
#include "algorithms/include/IterativeOptimization.h"

namespace scheduling_problem
{
	class SimulatedAnnealing : public ScheduleCorrector, public IterativeOptimization
	{
	public:
		enum class ReductionRules { boltzmann, couchy, mixed };

	private:
		enum class Params { MINTEMP, MAXTEMP };

		double minTemperature_, maxTemperature_;
		ReductionRules reductionRule_;
		std::function<double(double, std::size_t)> reduceTemperature_;
		std::unordered_map<std::string, Params> paramsMap_ = {
			{ "minTemp", Params::MINTEMP },
			{ "maxTemp", Params::MAXTEMP },
		};

	public:
		SimulatedAnnealing(double minTemp = 1, 
						   double maxTemp = 13, 
						   ReductionRules reductionRule = ReductionRules::boltzmann, 
						   BaseOptimization& baseline = BASELINE, 
						   unsigned saturation = 0, 
						   unsigned randomState = 42);

		virtual Schedule schedule(const Graph& graph);

		unsigned itersCount();

		void setParams(std::unordered_map<std::string, double> params);

	protected:
		bool isTransitionAcceptance(double energyDelta, double temperature);

		static double boltzmannRule(double initTemperature, std::size_t iterNum);

		static double couchyRule(double initTemperature, std::size_t iterNum);

		static double mixedRule(double initTemperature, std::size_t iterNum);

	private:
	};
}