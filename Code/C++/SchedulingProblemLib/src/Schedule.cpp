#include "include/Schedule.h"

namespace scheduling_problem
{
	Schedule::Schedule(std::size_t scheduleSize) : 
		std::vector<std::pair<std::size_t, weight_t>>(scheduleSize), bufferSize_(scheduleSize)
	{
		
	}

	Schedule::Schedule(const Schedule& schedule) : 
		std::vector<std::pair<std::size_t, weight_t>>(schedule)
	{
		currentSize_ = schedule.currentSize();
		bufferSize_ = schedule.bufferSize();
		cost_ = schedule.cost();
	}

	void Schedule::append(std::size_t vertexId, weight_t currentCost)
	{
		if (currentSize_ < bufferSize_) {
			this->operator[](currentSize_++) = std::pair<std::size_t, weight_t>(vertexId, currentCost);
			if (currentCost > cost_) {
				cost_ = currentCost;
			}
		}
	}

	std::size_t Schedule::currentSize() const
	{
		return currentSize_;
	}

	std::size_t Schedule::bufferSize() const
	{
		return bufferSize_;
	}

	weight_t Schedule::cost() const
	{
		return cost_;
	}

	void Schedule::recomputeCost()
	{
		cost_ = 0;
		for (auto pair : *this) {
			if (pair.second > cost_) {
				cost_ = pair.second;
			}
		}
	}
}

