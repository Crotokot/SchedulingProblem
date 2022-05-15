#pragma once

#include "include/additionals.h"
#include "algorithms/include/IterativeOptimization.h"
#include "experiments/GridSearch.h"

namespace scheduling_problem
{
	namespace experiments
	{
		std::unordered_map<std::string, additionals::Table> 
			epochsValSelection(std::unordered_map<std::string, BaseOptimization*> algorithms,
			additionals::DAGPool* dagPool, const std::vector<unsigned>& epochSet, std::string path);

		std::unordered_map<std::string, additionals::Table> 
			runsNumberSelection(std::unordered_map<std::string, BaseOptimization*> algorithms,
			additionals::DAGPool* dagPool, const std::vector<unsigned>& dupSet, std::string path);

		std::unordered_map<std::string, additionals::Table>
			algorithmsTesting(std::unordered_map<std::string, BaseOptimization*> algorithms,
			additionals::DAGPool* dagPool, unsigned duplicates, std::string path);

		void gridSearch(BaseOptimization* optimizer,
			const std::unordered_map<std::string, std::vector<double>>& paramGrid, additionals::DAGPool* dagPool, 
			unsigned duplicates, std::string path, unsigned nThreads = 1);
	}
}

