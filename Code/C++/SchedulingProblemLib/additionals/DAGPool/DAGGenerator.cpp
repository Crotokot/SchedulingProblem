#include "DAGGenerator.h"

namespace scheduling_problem
{
	namespace additionals
	{
        // =====================Random=======================

        DAGGenerator::Random::Random(const std::pair<std::size_t, std::size_t>& nVertices,
                                     const std::pair<weight_t, weight_t>& weights, 
                                     unsigned seed)
                                     : rng_(seed)
                                     , vertices_(nVertices.first, 
                                                 nVertices.second)
                                     , weights_(weights.first, 
                                                weights.second)
        {

        }

        DAGGenerator::Random::Random(const std::pair<std::size_t, std::size_t>& nVertices,
                                     const std::pair<double, double>& density, 
                                     const std::pair<weight_t, weight_t>& weights, 
                                     unsigned seed) 
                                     : Random(nVertices, weights, seed)
        {
            density_ = std::uniform_real_distribution<double>(density.first, density.second);
            continuousDensity_ = true;
        }

        DAGGenerator::Random::Random(const std::pair<std::size_t, std::size_t>& nVertices,
                                     const std::vector<double>& density, 
                                     const std::pair<weight_t, weight_t>& weights, 
                                     unsigned seed) 
                                     : Random(nVertices, weights, seed)
        {
            densitySet_ = std::vector<double>(density);
            continuousDensity_ = false;
        }

        std::size_t DAGGenerator::Random::vertices()
        {
            return vertices_(rng_);
        }

        weight_t DAGGenerator::Random::weights()
        {
            return weights_(rng_);
        }

        double DAGGenerator::Random::density()
        {
            return continuousDensity_ ? density_(rng_) :
                densitySet_[std::uniform_int_distribution<std::size_t>(0, densitySet_.size() - 1)(rng_)];
        }

        std::size_t DAGGenerator::Random::randUInt(std::size_t min, std::size_t max)
        {
            return std::uniform_int_distribution<std::size_t>(min, max)(rng_);
        }

        // =====================DAGGenerator=======================

		DAGGenerator::DAGGenerator(const std::pair<std::size_t, std::size_t>& nVertices, 
                                   const std::pair<double, double>& density, 
                                   const std::pair<weight_t, weight_t>& weights, 
                                   unsigned nSamples, 
                                   unsigned batchSize, 
                                   unsigned seed,
                                   std::string prefix)
                                   : DAGPool(nSamples, 
                                             batchSize)
                                   , random_(nVertices, 
                                             density, 
                                             weights, 
                                             seed)
                                   , prefix_(prefix)
		{

		}

        DAGGenerator::DAGGenerator(const std::pair<std::size_t, std::size_t>& nVertices,
                                   const std::vector<double>& density, 
                                   const std::pair<weight_t, weight_t>& weights,
                                   unsigned nSamples, 
                                   unsigned batchSize, 
                                   unsigned seed,
                                   std::string prefix)
                                   : DAGPool(nSamples, 
                                             batchSize)
                                   , random_(nVertices, 
                                             density, 
                                             weights, 
                                             seed)
                                   , prefix_(prefix)
        {

        }

		unsigned DAGGenerator::nextBatch(std::vector<std::pair<std::string, Graph>>& graphs)
		{
			unsigned sample(0);
			while (currentSample_ < nSamples_ && sample < batchSize_) {
                graphs[sample].first = prefix_ + std::to_string(currentSample_);
				graphs[sample++].second = makeGraph();
                currentSample_++;
			}
			return sample;
		}

        scheduling_problem::Graph DAGGenerator::makeGraph()
        {
            double density = random_.density();
            auto nVertex = random_.vertices(), 
                nEdge = (std::size_t)(density * ((nVertex - 1) * (nVertex - 2) / 2));
            std::uniform_int_distribution<std::size_t> vertexDist(0, nVertex - 1);

            auto weights = std::vector<scheduling_problem::weight_t>(nVertex);
            std::for_each(weights.begin(), weights.end(),
                [&](auto& elem)
                {
                    elem = random_.weights();
                });

            auto adjacency_matrix = std::vector<std::vector<std::size_t>>(nVertex);
            std::fill(adjacency_matrix.begin(), adjacency_matrix.end(), std::vector<std::size_t>(nVertex));
            std::size_t currentEdgeCount = 0;

            // make connected component
            std::vector<std::size_t> connectedVertices, unconnectedVertices = std::vector<std::size_t>(nVertex);
            std::iota(unconnectedVertices.begin(), unconnectedVertices.end(), 0);
            while (unconnectedVertices.size()) {
                std::size_t source, target;
                auto unconnVertexInd = random_.randUInt(0, unconnectedVertices.size() - 1);
                target = unconnectedVertices[unconnVertexInd];
                unconnectedVertices.erase(unconnectedVertices.begin() + unconnVertexInd);
                if (connectedVertices.size()) {
                    auto connVertexInd = random_.randUInt(0, connectedVertices.size() - 1);
                    source = connectedVertices[connVertexInd];
                    connectedVertices.push_back(unconnVertexInd);
                }
                else {
                    connectedVertices.push_back(unconnVertexInd);
                    unconnVertexInd = random_.randUInt(0, unconnectedVertices.size() - 1);
                    source = unconnectedVertices[unconnVertexInd];
                    unconnectedVertices.erase(unconnectedVertices.begin() + unconnVertexInd);
                }
                adjacency_matrix[source][target] = adjacency_matrix[target][source] = weights[std::min({ source, target })];
                currentEdgeCount++;
            }

            // Adding the remaining edges to the adjacency matrix 
            while (currentEdgeCount < nEdge) {
                auto source = random_.randUInt(0, nVertex - 1);
                auto target = random_.randUInt(0, nVertex - 1);
                if (source != target && adjacency_matrix[source][target] == 0) {
                    adjacency_matrix[source][target] = adjacency_matrix[target][source] = weights[std::min({ source, target })];
                    currentEdgeCount++;
                }
            }

            // make graph from adjacency matrix
            scheduling_problem::Graph graph(nVertex);
            auto weight_map = boost::get(scheduling_problem::vertex_weight_t(), graph);
            for (std::size_t i = 0; i < nVertex; i++) {
                for (std::size_t j = i + 1; j < nVertex; j++) {
                    if (adjacency_matrix[i][j]) {
                        boost::add_edge(i, j, graph);
                    }
                }
                auto iters = boost::out_edges(i, graph);
                weight_map[i] = iters.second - iters.first > 0 ? weights[i] : 0;
            }
            return graph;
        }
	}
}