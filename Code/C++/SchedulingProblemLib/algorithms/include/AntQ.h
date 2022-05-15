#pragma once

#include "AntColonyOptimization.h"

namespace scheduling_problem
{
	class AntQ : public AntColonyOptimization
	{
	private:

		enum class Params { GAMMA, W };

		std::unordered_map<std::string, Params> paramsMap_ = {
			{ "gamma", Params::GAMMA },
			{ "W", Params::W }
		};

		double gamma_, W_;
	public:
		AntQ(double pheDecay = 0.1, 
			 double threshold = 0.4, 
			 double pheInfluence = 0.6, 
			 double heuInfluence = 0.8, 
			 double gamma = 0.2, 
			 double W = 0.5,
			 unsigned epochs = 100, 
			 unsigned ants = 5, 
			 BaseOptimization& baseline = BASELINE, 
			 unsigned saturation = 0, 
			 unsigned randomState = 42);

		virtual Schedule schedule(const Graph& graph);

		void setParams(std::unordered_map<std::string, double> params);

	protected:

		void localAQValueUpdate(std::size_t vertexID, std::size_t step, 
			const std::unordered_set<std::size_t>& available);

		void globalAQValueUpdate(const Graph& graph, const Schedule& solution, 
			VertexTiesMap& vertexTies, bool isBest = false);

		double getMaxAQ(std::size_t pos, const std::unordered_set<std::size_t>& available);

		std::vector<Schedule> completeEpoch(const Graph& graph, 
			VertexTiesMap& vertexTies, std::size_t nSteps, double initTrail);
	};
}

