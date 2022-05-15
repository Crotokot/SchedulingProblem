#include "DAGPool.h"

namespace scheduling_problem
{
	namespace additionals
	{
		DAGPool::DAGPool(unsigned nSamples,
						 unsigned batchSize)
						 : nSamples_(nSamples)
						 , batchSize_(batchSize)
						 , currentSample_(0) 
		{
			
		}

		unsigned DAGPool::batchSize() 
		{ 
			return batchSize_; 
		}

		unsigned DAGPool::samplesNum()
		{
			return nSamples_;
		}
	}
}