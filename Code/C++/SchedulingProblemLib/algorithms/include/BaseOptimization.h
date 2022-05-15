#pragma once

#include <random>
#include <chrono>
#include "include/general_types.h"
#include "include/Schedule.h"

namespace scheduling_problem 
{
	class BaseOptimization
	{
	protected:
		int randomState_;
		std::mt19937 rng_;
		long long duration_;

	public:
		BaseOptimization(unsigned randomState = 42);

		virtual Schedule schedule(const Graph& graph);

		void setParams(const std::unordered_map<std::string, double>& params);

		long long duration() const;

	protected:
		VertexTiesMap initializeMemory(const Graph& graph);

		std::pair<IdsSetsMap, Schedule> makeState(const Graph& graph, VertexTiesMap& vertexTies);
		/*
			* Fills buffers for exterior properties and create two sets of 
			* available and memorized tasks and solution instance.
		*/

		void updateState(const Graph& graph, IdsSetsMap& vertexIdS,
			Schedule& solution, VertexTiesMap& vertexTies, std::size_t vertexID);
		/*
			* Updates sets of availabels and memorized tasks, solution sequence and 
			* exterior properties (vertex_child_cnt and vertex_parent_cnt)
		*/

		std::size_t choice(const std::unordered_set<std::size_t>& availableIds);

		weight_t solutionCost(const Graph& graph, const std::unordered_set<std::size_t>& memorizedIds);

		std::chrono::steady_clock::time_point makeTimePoint() const;

		void calculateDuration(const std::pair<std::chrono::steady_clock::time_point, 
			std::chrono::steady_clock::time_point>& segment);
	};
}