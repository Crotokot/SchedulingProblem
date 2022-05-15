#pragma once

#include <random>
#include <numeric>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "additionals/TableContent.h"
#include "additionals/ProgressBar.h"
#include "additionals/DAGPool/DAGReader.h"
#include "additionals/DAGPool/DAGGenerator.h"
#include "algorithms/include/BaseOptimization.h"

namespace scheduling_problem
{	
	namespace additionals
	{

		scheduling_problem::Graph makeGraph(std::size_t nVertex, std::size_t nEdge, 
			scheduling_problem::weight_t weight, unsigned seed);

		void printNetwork(const scheduling_problem::Graph& graph);

		void printSolution(scheduling_problem::Schedule& solution, bool full = true);


		typedef TableContent<std::string, std::string, scheduling_problem::weight_t> Table;

		std::unordered_map<std::string, Table> process(const std::vector<scheduling_problem::Graph>& graphs,
			std::unordered_map<std::string, scheduling_problem::BaseOptimization*> algorithms, 
			std::size_t& batchID, std::size_t duplicates = 6, std::string path = "./");

		void serializeGraph(const Graph& graph, std::string path);
	}
}