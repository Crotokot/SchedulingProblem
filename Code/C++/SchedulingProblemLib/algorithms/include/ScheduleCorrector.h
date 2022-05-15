#pragma once

#include "algorithms/include/BaseOptimization.h"

namespace scheduling_problem
{
	class ScheduleCorrector : virtual public BaseOptimization
	{
	/*private:
		std::mt19937 rng_;*/

	public:
		//ScheduleCorrector(std::mt19937 rng);

		ScheduleCorrector(unsigned randomState);

		Schedule correct(const Graph& graph, const Schedule& solution,
			std::unordered_map<std::string, std::vector<std::size_t>>& bounds);

		std::unordered_map<std::string, std::vector<std::size_t>>
			makeBounds(const Graph& graph, const Schedule& solution) const;

	private:
		std::size_t choice(std::unordered_map<std::string, std::vector<std::size_t>>& bounds);

		std::size_t selectPosition(std::size_t vertexID, 
			std::unordered_map<std::string, std::vector<std::size_t>>& bounds);

		void transition(const Graph& graph, Schedule& solution, const std::size_t vertexID, 
			const std::size_t tpos, std::unordered_map<std::string, std::vector<std::size_t>>& bounds);
	
		//void correctBounds();
	
	};
}