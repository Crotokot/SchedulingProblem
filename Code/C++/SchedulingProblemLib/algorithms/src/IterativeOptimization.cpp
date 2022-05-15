#include "algorithms/include/IterativeOptimization.h"

namespace scheduling_problem
{
	IterativeOptimization::IterativeOptimization(BaseOptimization& baseline, 
												 unsigned saturation) 
												 : baseline_(baseline)
												 , saturation_(saturation)
												 , itersCount_(0)
	{
		
	}

	unsigned IterativeOptimization::itersCount()
	{
		return itersCount_;
	}

	void IterativeOptimization::setParams(const std::unordered_map<std::string, double>& params)
	{
		for (auto param : params) {
			switch (paramsMap_[param.first]) {
				case Params::SATURATION:
					saturation_ = (unsigned)param.second;
					break;
			}
		}
	}
}