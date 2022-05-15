#pragma once

#include "algorithms/include/BaseOptimization.h"

namespace scheduling_problem
{
	class CorrectnessTester : public BaseOptimization
	{
	public:
		static bool check(const Schedule& schedule, const Schedule& solution, VertexTiesMap vertexTies);

		static bool checkPrecedence(const Graph& graph, const Schedule& schedule);

		bool isCorrect(const scheduling_problem::Graph& graph, const scheduling_problem::Schedule& schedule);
	};
}