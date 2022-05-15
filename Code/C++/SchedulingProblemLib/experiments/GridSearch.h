#pragma once

#include "include/additionals.h"
#include "algorithms/include/AntColonyOptimization.h"

namespace scheduling_problem
{
	namespace experiments
	{
		class GridSearch
		{

		private:
			unsigned duplicates_;
			BaseOptimization* optimizer_;
			std::unordered_map<std::string, std::vector<double>> paramGrid_;

		public:
			typedef additionals::TableContent<std::string, std::string, double> Table;

			GridSearch(BaseOptimization* optimizer, 
					   const std::unordered_map<std::string, std::vector<double>>& paramGrid, 
					   unsigned duplicates);

			std::unordered_map<std::string, Table> fit(additionals::DAGPool* dagPool, std::string path);

			void fit(additionals::DAGPool* dagPool, std::string path, unsigned nThreads);

		private:
			std::vector<std::unordered_map<std::string, double>> makeParamSets();
		};
	}
}

