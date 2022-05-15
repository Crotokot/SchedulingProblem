#pragma once

#include <vector>
#include "include/general_types.h"

namespace scheduling_problem
{
	class Schedule : public std::vector<std::pair<std::size_t, weight_t>>
	{
	private:
		std::size_t currentSize_ = 0;
		std::size_t bufferSize_ = 0;
		weight_t cost_ = 0;

	public:
		Schedule(std::size_t scheduleSize);

		Schedule(const Schedule& schedule);

		void append(std::size_t vertexId, weight_t vertexWeight);

		std::size_t currentSize() const;

		std::size_t bufferSize() const;

		weight_t cost() const;

		void recomputeCost();

	};
}