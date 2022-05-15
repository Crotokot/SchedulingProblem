#pragma once

#include <regex>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "additionals/DAGPool/DAGPool.h"

namespace scheduling_problem 
{
	namespace additionals 
	{
		class DAGReader : public DAGPool
		{
		private:
			std::string directory_;
			std::filesystem::recursive_directory_iterator current_, end_;
			std::vector<std::filesystem::path> paths_;

		public:
			DAGReader(std::string directory, 
					  unsigned nSamples, 
					  unsigned batchSize);

			virtual unsigned nextBatch(std::vector<std::pair<std::string, Graph>>& graphs);

		private:
			bool isDAG(std::ifstream& file);

			void make(std::ifstream& file, Graph& graph);

			std::string strip(const std::string& str);

			std::unordered_map<std::size_t, std::size_t> cleanData(std::vector<std::size_t>& vertices) const;
		};
	}
}


