#pragma once

#include "algorithms/include/ScheduleCorrector.h"
#include "algorithms/include/AntColonyOptimization.h"

namespace scheduling_problem
{
	class SUMIACO : public AntColonyOptimization, public ScheduleCorrector
	{
	private:

		enum class Params { LOCALSEARCHITERS };

		std::unordered_map<std::string, Params> paramsMap_ = {
			{ "localSearchIters", Params::LOCALSEARCHITERS }
		};

		unsigned localSearchIters_;

	public:
		SUMIACO(double pheDecay = 0.1, 
				double threshold = 0.4, 
				double pheInfluence = 0.6, 
				double heuInfluence = 0.8, 
				unsigned epochs = 100, 
				unsigned ants = 5, 
				BaseOptimization& baseline = BASELINE, 
				unsigned saturation = 0,
				unsigned randomState = 42, 
				unsigned localSearchIters = 20);

		virtual Schedule schedule(const Graph& graph);

		void setParams(std::unordered_map<std::string, double> params);

	private:
		void localSearch(const Graph& graph, Schedule& solution);

		void localSearchEpoch(const Graph& graph, std::vector<Schedule>& epochResults);
	};
}