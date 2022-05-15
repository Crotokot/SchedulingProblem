#include "include/additionals.h"

namespace scheduling_problem
{
	namespace additionals
	{
        scheduling_problem::Graph makeGraph(std::size_t nVertex, std::size_t nEdge, 
            scheduling_problem::weight_t weight, unsigned seed)
        {
            if (nEdge < (nVertex - 2) * (nVertex - 1) / 2 && nEdge >= nVertex - 1) {
                std::mt19937 rng(seed);
                std::uniform_int_distribution<std::size_t> vertexDist(0, nVertex - 1);
                std::uniform_int_distribution<std::size_t> weightDist(1, weight);

                auto weights = std::vector<scheduling_problem::weight_t>(nVertex);
                std::for_each(weights.begin(), weights.end(),
                    [&](auto& elem)
                    {
                        elem = weightDist(rng);
                    });

                auto adjacency_matrix = std::vector<std::vector<std::size_t>>(nVertex);
                std::fill(adjacency_matrix.begin(), adjacency_matrix.end(), std::vector<std::size_t>(nVertex));
                std::size_t currentEdgeCount = 0;

                // make connected component
                std::vector<std::size_t> connectedVertices, unconnectedVertices = std::vector<std::size_t>(nVertex);
                std::iota(unconnectedVertices.begin(), unconnectedVertices.end(), 0);
                while (unconnectedVertices.size()) {
                    std::size_t source, target;
                    auto unconnVertexInd = std::uniform_int_distribution<std::size_t>(0, unconnectedVertices.size() - 1)(rng);
                    target = unconnectedVertices[unconnVertexInd];
                    unconnectedVertices.erase(unconnectedVertices.begin() + unconnVertexInd);
                    if (connectedVertices.size()) {
                        auto connVertexInd = std::uniform_int_distribution<std::size_t>(0, connectedVertices.size() - 1)(rng);
                        source = connectedVertices[connVertexInd];
                        connectedVertices.push_back(unconnVertexInd);
                    }
                    else {
                        connectedVertices.push_back(unconnVertexInd);
                        unconnVertexInd = std::uniform_int_distribution<std::size_t>(0, unconnectedVertices.size() - 1)(rng);
                        source = unconnectedVertices[unconnVertexInd];
                        unconnectedVertices.erase(unconnectedVertices.begin() + unconnVertexInd);
                    }
                    adjacency_matrix[source][target] = adjacency_matrix[target][source] = weights[std::min({ source, target })];
                    currentEdgeCount++;

                }

                while (currentEdgeCount < nEdge) {
                    auto source = vertexDist(rng);
                    auto target = vertexDist(rng);
                    if (source != target && adjacency_matrix[source][target] == 0) {
                        adjacency_matrix[source][target] = adjacency_matrix[target][source] = weights[std::min({ source, target })];
                        currentEdgeCount++;
                    }
                }

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
            else {
                throw;
            }
        }

        void printNetwork(const scheduling_problem::Graph& graph)
        {
            auto vertices = boost::vertices(graph);
            for (auto vertex = vertices.first; vertex < vertices.second; vertex++) {
                std::cout << "Vertex: " << *vertex << std::endl;
                std::cout << "\tWeight: " << boost::get(scheduling_problem::vertex_weight_t(), graph, *vertex) << std::endl;
                auto out = boost::out_edges(*vertex, graph);
                std::for_each(out.first, out.second,
                    [&](auto edge)
                    {
                        auto target = boost::target(edge, graph);
                        std::cout << "\t" << *vertex << " ---> " << target << std::endl;;
                    });

                auto in = boost::in_edges(*vertex, graph);
                std::for_each(in.first, in.second,
                    [&](auto edge)
                    {
                        auto source = boost::source(edge, graph);
                        std::cout << "\t" << *vertex << " <--- " << source << std::endl;;
                    });
            }
        }

        void printSolution(scheduling_problem::Schedule& solution, bool full)
        {
            if (full) {
                for (std::size_t i = 0; i < solution.currentSize(); i++) {
                    std::cout << "Vetex: " << solution[i].first << "\n\tCost: " << solution[i].second << std::endl;
                }
            }
            std::cout << "Solution cost: " << solution.cost() << std::endl;
        }
        
        std::unordered_map<std::string, Table> process(const std::vector<scheduling_problem::Graph>& graphs,
            std::unordered_map<std::string, BaseOptimization*> algorithms, std::size_t& batchID, 
            std::size_t duplicates, std::string path)
        {
            std::vector<std::string> columns;
            for (std::size_t i(1); i <= duplicates; i++) {
                columns.push_back("cost_" + std::to_string(i));
            }
            for (std::size_t i(1); i <= duplicates; i++) {
                columns.push_back("time_" + std::to_string(i));
            }
            std::unordered_map<std::string, Table> results;
            std::vector<scheduling_problem::weight_t> graphResults(duplicates * 2);
            ProgressBar progress(40, (unsigned)(duplicates * graphs.size() - 1));
            for (auto& algorithm : algorithms) {
                progress.reset();
                auto label = algorithm.first + "_batch_";
                auto optimizer = algorithm.second;
                std::cout << algorithm.first << std::endl;

                Table table(columns);
                for (const auto& graph : graphs) {
                    std::size_t nEdge = boost::num_edges(graph);
                    std::size_t nVertex = boost::num_vertices(graph);
                    auto dim = "(" + std::to_string(nVertex) + ", " + std::to_string(nEdge) + ")";
                    for (std::size_t i(0); i < duplicates; i++) {
                        auto solution = optimizer->schedule(graph);
                        graphResults[i] = solution.cost();
                        graphResults[i + duplicates] = optimizer->duration();
                        progress.show();
                    }
                    table.append(dim, graphResults);
                }
                auto batchLabel = label + std::to_string(batchID++);
                table.to_csv(path + batchLabel + ".csv");
                results[batchLabel] = table;

            }
            return results;
        }

        void serializeGraph(const Graph& graph, std::string path)
        {
            std::ofstream file(path);
            file << "node\tsize\tchildren\n";
            auto vertices = boost::vertices(graph);
            for (auto vertex = vertices.first; vertex < vertices.second; vertex++) {
                file << *vertex << "\t" << 
                    boost::get(scheduling_problem::vertex_weight_t(), graph, *vertex);
                auto children = boost::adjacent_vertices(*vertex, graph);
                for (auto child = children.first; child < children.second; child++) {
                    file << "\t" << *child;
                }
                file << std::endl;
            }
            file.close();
        }
	}
}