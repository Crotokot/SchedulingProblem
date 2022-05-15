#pragma once

#include <random>
#include <numeric>
#include "include/general_types.h"
#include "additionals/DAGPool/DAGPool.h"

namespace scheduling_problem
{
	namespace additionals
	{
		class DAGGenerator : public DAGPool
		{
		private:

			struct Random
			{
			private:
				std::mt19937 rng_;
				std::uniform_int_distribution<std::size_t> vertices_;
				std::uniform_int_distribution<weight_t> weights_;
				std::uniform_real_distribution<double> density_;
				std::vector<double> densitySet_;
				bool continuousDensity_;
				
				Random(const std::pair<std::size_t, std::size_t>& nVertices,
					   const std::pair<weight_t, weight_t>& weights, 
					   unsigned seed);
			
			public:

				Random(const std::pair<std::size_t, std::size_t>& nVertices,
				       const std::pair<double, double>& density,
					   const std::pair<weight_t, weight_t>& weights, 
					   unsigned seed);

				Random(const std::pair<std::size_t, std::size_t>& nVertices,
					   const std::vector<double>& density,
					   const std::pair<weight_t, weight_t>& weights,
					   unsigned seed);

				std::size_t vertices();

				weight_t weights();

				double density();

				std::size_t randUInt(std::size_t min, std::size_t max);

			};

			std::vector<double> density_;
			std::string prefix_;
			Random random_;

		public:
			DAGGenerator(const std::pair<std::size_t, std::size_t>& nVertices, 
						 const std::pair<double, double>& density, 
						 const std::pair<weight_t, weight_t>& weights, 
						 unsigned nSamples, 
						 unsigned batchSize, 
						 unsigned seed,
						 std::string prefix = "dag_");

			DAGGenerator(const std::pair<std::size_t, std::size_t>& nVertices,
						 const std::vector<double>& density, 
						 const std::pair<weight_t, weight_t>& weights,
						 unsigned nSamples, 
						 unsigned batchSize, 
						 unsigned seed,
						 std::string prefix = "dag_");

			virtual unsigned nextBatch(std::vector<std::pair<std::string, Graph>>& graphs);

		private:
			Graph makeGraph();
		};
	}
}


