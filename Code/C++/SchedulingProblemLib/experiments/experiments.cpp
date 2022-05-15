#include "experiments.h"

namespace scheduling_problem
{
	namespace experiments
	{
		std::unordered_map<std::string, scheduling_problem::additionals::Table> 
			runsNumberSelection(std::unordered_map<std::string, BaseOptimization*> algorithms,
								scheduling_problem::additionals::DAGPool* dagPool, 
								const std::vector<unsigned>& dupSet, 
								std::string path)
		{
			std::vector<std::pair<std::string, Graph>> graphs(dagPool->batchSize());
			unsigned batchNum = 1, shift = 3;
			std::unordered_map<std::string, scheduling_problem::additionals::Table> results;
			while (dagPool->nextBatch(graphs)) {
				for (auto optimizer : algorithms) {
					std::cout << optimizer.first << std::endl;
					scheduling_problem::additionals::ProgressBar 
						progress(60, (unsigned)(graphs.size() * dupSet.size() - 1));
					auto iterativeOptimizer = dynamic_cast<IterativeOptimization*>(optimizer.second);
					for (auto duplicates : dupSet) {
						std::vector<std::string> columns(duplicates * 3 + 3);
						columns[0] = "duplicates";
						columns[1] = "n_vertex";
						columns[2] = "n_edges";
						for (std::size_t i(shift); i < duplicates + shift; i++) {
							auto num = std::to_string(i);
							columns[i] = "cost_" + num;
							columns[i + duplicates] = "time_" + num;
							columns[i + duplicates * 2] = "iters_" + num;
						}
						std::vector<weight_t> graphResults(columns.size());
						scheduling_problem::additionals::Table table(columns);
						graphResults[0] = duplicates;
						for (auto graph : graphs) {
							graphResults[1] = boost::num_vertices(graph.second);
							graphResults[2] = boost::num_edges(graph.second);
							for (unsigned i(shift); i < duplicates + shift; i++) {
								auto solution = optimizer.second->schedule(graph.second);
								graphResults[i] = solution.cost();
								graphResults[i + duplicates] = optimizer.second->duration();
								graphResults[i + duplicates * 2] =
									iterativeOptimizer != nullptr ? iterativeOptimizer->itersCount() : 1;
							}
							table.append(graph.first, graphResults);
							progress.show();
						}
						std::string label = optimizer.first + "_dup_" + 
							std::to_string(duplicates) + "_batch_" + std::to_string(batchNum);
						table.to_csv(path + "/" + label + ".csv");
						results[label] = table;
					}
				}
				batchNum++;
			}
			return results;
		}

		std::unordered_map<std::string, scheduling_problem::additionals::Table>
			algorithmsTesting(std::unordered_map<std::string, BaseOptimization*> algorithms,
			scheduling_problem::additionals::DAGPool* dagPool, unsigned duplicates, std::string path)
		{
			std::vector<std::pair<std::string, Graph>> graphs(dagPool->batchSize());
			std::vector<std::string> columns(duplicates * 3 + 3);
			columns[0] = "duplicates";
			columns[1] = "n_vertex";
			columns[2] = "n_edges";
			unsigned shift = 3;
			for (std::size_t i(shift); i < duplicates + shift; i++) {
				auto num = std::to_string(i);
				columns[i] = "cost_" + num;
				columns[i + duplicates] = "time_" + num;
				columns[i + duplicates * 2] = "iters_" + num;
			}
			std::vector<weight_t> graphResults(columns.size());
			graphResults[0] = duplicates;
			scheduling_problem::additionals::ProgressBar progress(60, (unsigned)(duplicates * graphs.size() - 1));
			unsigned batchNum = 1;
			std::unordered_map<std::string, scheduling_problem::additionals::Table> results;
			while (dagPool->nextBatch(graphs)) {
				for (auto optimizer : algorithms) {
					progress.reset();
					scheduling_problem::additionals::Table table(columns);
					std::cout << optimizer.first << std::endl;
					auto iterativeOptimizer = dynamic_cast<IterativeOptimization*>(optimizer.second);

					for (auto graph : graphs) {
						graphResults[1] = boost::num_vertices(graph.second);
						graphResults[2] = boost::num_edges(graph.second);
						for (std::size_t i(shift); i < duplicates + shift; i++) {
							auto solution = optimizer.second->schedule(graph.second);
							graphResults[i] = solution.cost();
							graphResults[i + duplicates] = optimizer.second->duration();
							graphResults[i + duplicates * 2] =
								iterativeOptimizer != nullptr ? iterativeOptimizer->itersCount() : 1;
							progress.show();
						}
						table.append(graph.first, graphResults);
					}
					std::string label = optimizer.first + "_results_batch_" + std::to_string(batchNum);
					table.to_csv(path + "/" + label + ".csv");
					results[label] = table;
				}
				batchNum++;
			}
			return results;
		}

		void gridSearch(BaseOptimization* optimizer,
			const std::unordered_map<std::string, std::vector<double>>& paramGrid, additionals::DAGPool* dagPool,
			unsigned duplicates, std::string path, unsigned nThreads)
		{
			GridSearch searcher(optimizer, paramGrid, duplicates);
			searcher.fit(dagPool, path, nThreads);
		}

	}
}