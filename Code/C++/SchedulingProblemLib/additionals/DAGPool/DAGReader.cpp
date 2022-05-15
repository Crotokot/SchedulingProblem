#include "DAGReader.h"
#include <iostream>

namespace scheduling_problem
{
	namespace additionals
	{
		DAGReader::DAGReader(std::string directory, 
							 unsigned nSamples, 
							 unsigned batchSize) 
							 : DAGPool(nSamples, batchSize)
							 , directory_(directory)
		{
			std::filesystem::recursive_directory_iterator current(directory_), end;
			for (; current != end; current++) {
				paths_.push_back(current->path());
			}
			std::sort(paths_.begin(), paths_.end());
		}

		unsigned DAGReader::nextBatch(std::vector<std::pair<std::string, Graph>>& graphs)
		{
			unsigned sample(0);
			while (currentSample_ < nSamples_ && currentSample_ < paths_.size() && sample < batchSize_) {
				if (paths_[currentSample_].extension() == ".txt") {
					std::string filename = paths_[currentSample_].string();
					std::ifstream file(filename);
					if (isDAG(file)) {
						make(file, graphs[sample].second);
						graphs[sample++].first = paths_[currentSample_].stem().string();
						currentSample_++;
					}
					file.close();
				}
			}
			return sample;
		}

		bool DAGReader::isDAG(std::ifstream& file)
		{
			bool result = false;
			std::string node, size, children;
			file >> node >> size >> children;
			if (node == "node" && size == "size" && children == "children") {
				result = true;
			}
			return result;
		}

		void DAGReader::make(std::ifstream& file, Graph& graph)
		{
			std::unordered_set<std::size_t> vertices;
			std::unordered_map<std::size_t, std::pair<scheduling_problem::weight_t, std::vector<std::size_t>>> data;
			while (!file.eof()) {
				std::stringstream stream;
				std::string str;
				std::getline(file, str);
				str = strip(str);
				if (str.size() > 1) {
					stream << str;
					std::size_t node, size;
					std::vector<std::size_t> children;
					stream >> node >> size;
					vertices.insert(node);
					while (stream.rdbuf()->in_avail()) {
						std::size_t child;
						stream >> child;
						children.push_back(child);
					}
					data[node] = { size, children };
				}
			}

			std::vector<std::size_t> verts(vertices.begin(), vertices.end());
			auto verticesMap = cleanData(verts);

			std::size_t nVertex = vertices.size();
			graph = scheduling_problem::Graph(nVertex);
			auto weight_map = boost::get(scheduling_problem::vertex_weight_t(), graph);
			for (auto& vertexInfo : data) {
				auto vertexID = verticesMap[vertexInfo.first];
				for (auto& child : vertexInfo.second.second) {
					auto childID = verticesMap[child];
					boost::add_edge(vertexID, childID, graph);
				}
				weight_map[vertexID] = vertexInfo.second.first;
			}
		}

		std::string DAGReader::strip(const std::string& str)
		{
			return std::regex_replace(str, std::regex("^[ |\t|\n|\r|\v|\f]*|[ |\t|\n|\r|\v|\f]*$"), "");
		}

		std::unordered_map<std::size_t, std::size_t> DAGReader::cleanData(std::vector<std::size_t>& vertices) const
		{
			std::unordered_map<std::size_t, std::size_t> vertexMap;
			std::sort(vertices.begin(), vertices.end());
			for (std::size_t ind(0); ind < vertices.size(); ind++) {
				vertexMap[vertices[ind]] = ind;
			}
			return vertexMap;
		}
	}
}
