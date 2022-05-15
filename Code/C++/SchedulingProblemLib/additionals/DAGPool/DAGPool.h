#pragma once

#include <vector>
#include "include/general_types.h"

namespace scheduling_problem
{
	namespace additionals
	{
		class DAGPool
		{
		protected:
			unsigned batchSize_, nSamples_, currentSample_;

		public:
			DAGPool(unsigned nSamples,
					unsigned batchSize);

			virtual unsigned nextBatch(std::vector<std::pair<std::string, Graph>>& graphs) = 0;

			unsigned batchSize();

			unsigned samplesNum();
		};
	}
}