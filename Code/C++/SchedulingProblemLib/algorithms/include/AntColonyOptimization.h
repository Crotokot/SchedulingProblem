#pragma once

#include <numeric>
#include "algorithms/include/GreedyOptimization.h"
#include "algorithms/include/IterativeOptimization.h"

namespace scheduling_problem
{
	//extern BaseOptimization BASELINE;

	class AntColonyOptimization : public GreedyOptimization, public IterativeOptimization
	{
	private:
		
		enum class Params { PHEDECAY, THRESHOLD, PHEINFLUENCE, HEUINFLUENCE, ANTS, EPOCHS };

		std::unordered_map<std::string, Params> paramsMap_ = {
			{ "pheDecay", Params::PHEDECAY },
			{ "threshold", Params::THRESHOLD },
			{ "pheInfluence", Params::PHEINFLUENCE },
			{ "heuInfluence", Params::HEUINFLUENCE },
			{ "ants", Params::ANTS },
			{ "epochs", Params::EPOCHS },
		};

	protected:
		double pheDecay_, threshold_, pheInfluence_, heuInfluence_;
		unsigned ants_, epochs_;
		std::vector<std::vector<double>> weightMatrix_;

	public:

		AntColonyOptimization(double pheDecay = 0.1, 
							  double threshold = 0.4, 
							  double pheInfluence = 0.6, 
							  double heuInfluence = 0.8, 
							  unsigned epochs = 100, 
							  unsigned ants = 5, 
							  BaseOptimization& baseline = BASELINE, 
							  unsigned saturation = 0, 
							  unsigned randomState = 42);

		virtual Schedule schedule(const Graph& graph);

		void setParams(double pheDecay, double threshold, double pheInfluence, double heuInfluence);

		void setParams(std::unordered_map<std::string, double> params);

	protected:
		void initWeightMatrix(std::size_t nVertex, double initTrail);

		std::vector<Schedule> completeEpoch(const Graph& graph, 
			VertexTiesMap& vertexTies, std::size_t nSteps, double initTrail);

		std::size_t choice(const Graph& graph, IdsSetsMap& vertexIdS, VertexTiesMap& vertexTies, std::size_t step);

		Schedule selectBestEpochSolution(std::vector<Schedule>& epochResults);

		void localPheromoneUpdate(std::size_t vertexID, std::size_t step, double initTrail);

		void globalPheromoneUpdate(const Schedule& bestSolution, bool isBest = false);

		std::size_t hypergeometricVal(std::unordered_map<std::size_t, double>& probs);
	};
}