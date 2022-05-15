#include "GridSearch.h"

#include <thread>
#include <iostream>

namespace scheduling_problem
{
	namespace experiments
	{
		GridSearch::GridSearch(BaseOptimization* optimizer,
							   const std::unordered_map<std::string, std::vector<double>>& paramGrid, 
							   unsigned duplicates) 
							   : paramGrid_(paramGrid)
							   , duplicates_(duplicates) 
		{
			optimizer_ = optimizer;
		}

		std::unordered_map<std::string, GridSearch::Table> GridSearch::fit(additionals::DAGPool* dagPool, std::string path)
		{
			std::size_t paramsCount = paramGrid_.size();
			std::vector<std::string> columns(paramsCount + 3 + duplicates_ * 3);
			columns[0] = "duplicates";
			columns[1] = "n_vertex";
			columns[2] = "n_edges";
			unsigned shift(3);
			for (auto param : paramGrid_) {
				columns[shift++] = param.first;
			}
			for (unsigned i(shift); i < duplicates_ + shift; i++) {
				auto num = std::to_string(i - shift + 1);
				columns[i] = "cost_" + num;
				columns[i + duplicates_] = "time_" + num;
				columns[i + duplicates_ * 2] = "iters_" + num;
			}
			std::unordered_map<std::string, Table> results;
			auto paramSets = makeParamSets();
			additionals::ProgressBar progress(60, (unsigned)(dagPool->batchSize() * paramSets.size() * duplicates_ - 1));
			std::vector<std::pair<std::string, Graph>> graphs(dagPool->batchSize());
			std::vector<double> graphResults(columns.size());
			graphResults[0] = duplicates_;
			unsigned batchNum = 1;
			while (dagPool->nextBatch(graphs)) {
				Table table(columns);
				progress.reset();
				for (auto params : paramSets) {
					for (unsigned i(3); i < shift; i++) {
						graphResults[i] = params[columns[i]];
					}
					optimizer_->setParams(params);
					auto iterativeOptimizer = dynamic_cast<IterativeOptimization*>(optimizer_);
					for (auto graph : graphs) {
						graphResults[1] = (double)boost::num_edges(graph.second);
						graphResults[2] = (double)boost::num_vertices(graph.second);
						for (unsigned i(shift); i < duplicates_ + shift; i++) {
							auto solution = optimizer_->schedule(graph.second);
							graphResults[i] = (double)solution.cost();
							graphResults[i + duplicates_] = (double)(optimizer_->duration());
							graphResults[i + duplicates_ * 2] =
								iterativeOptimizer != nullptr ? iterativeOptimizer->itersCount() : 1;
							progress.show();
						}
						table.append(graph.first, graphResults);
					}
				}
				std::string label = "GridSearch_batch_" + std::to_string(batchNum);
				results[label] = table;
				table.to_csv(path + "/" + label + ".csv");
				batchNum++;
			}
			return results;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////

		void task(std::vector<std::pair<std::string, Graph>> graphs,
				  AntColonyOptimization optimizer, 
				  std::vector<std::unordered_map<std::string, double>> paramSets, 
				  std::vector<std::string> columns,
				  std::string path,
				  unsigned duplicates,
				  unsigned batchNum,
				  int& completed)
		{
			GridSearch::Table table(columns);
			std::vector<double> graphResults(columns.size());
			graphResults[0] = duplicates;
			auto id = paramSets[0].size() + 1;
			for (auto params : paramSets) {
				for (unsigned i(1); i < id; i++) {
					graphResults[i] = params[columns[i]];
				}
				optimizer.setParams(params);
				for (auto graph : graphs) {
					std::size_t nEdge = boost::num_edges(graph.second);
					std::size_t nVertex = boost::num_vertices(graph.second);
					
					for (unsigned i(id); i < duplicates + id; i++) {
						auto solution = optimizer.schedule(graph.second);
						graphResults[i] = (double)solution.cost();
						graphResults[i + duplicates] = (double)(optimizer.duration());
						graphResults[i + duplicates * 2] = optimizer.itersCount();
					}
					table.append(graph.first, graphResults);
				}
			}
			std::string label = "GridSearch_batch_" + std::to_string(batchNum);
			table.to_csv(path + "/" + label + ".csv");
			completed = 2;
		}


		void GridSearch::fit(additionals::DAGPool* dagPool, std::string path, unsigned nThreads)
		{
			std::size_t paramsCount = paramGrid_.size();
			std::vector<std::string> columns(paramsCount + 1 + duplicates_ * 3);
			columns[0] = "duplicates";
			unsigned id(1);
			for (auto param : paramGrid_) {
				columns[id++] = param.first;
			}
			for (unsigned i(id); i < duplicates_ + id; i++) {
				auto num = std::to_string(i - id + 1);
				columns[i] = "cost_" + num;
				columns[i + duplicates_] = "time_" + num;
				columns[i + duplicates_ * 2] = "iters_" + num;
			}
			auto paramSets = makeParamSets();
			unsigned batchSize = dagPool->batchSize(), nSamples = dagPool->samplesNum();
			unsigned nBatches = nSamples / batchSize + (nSamples % batchSize ? 1 : 0);
			std::vector<std::pair<std::string, Graph>> graphs(batchSize);
			std::vector<double> graphResults(columns.size());
			graphResults[0] = duplicates_;
			unsigned batchNum = 1;

			std::vector<std::thread> threads(nThreads);
			std::vector<int> completed(nThreads);
			std::fill(completed.begin(), completed.end(), 0);
			unsigned threadNum = 0;
			unsigned completedCount = 0, processedBatchCount = 0;
			while (completedCount < nBatches) {
				for (unsigned i(0); i < nThreads; i++) {
					if (processedBatchCount < nBatches) {
						if (completed[i] == 0) {
							completed[i] = 1;
							processedBatchCount++;
							dagPool->nextBatch(graphs);
							threads[i] = std::move(std::thread(task, graphs, 
								AntColonyOptimization(*dynamic_cast<AntColonyOptimization*>(optimizer_)),
								paramSets, columns, path, duplicates_, batchNum++, std::ref(completed[i])));
							
							std::cout << "Thread " << i << " started!" << std::endl;
						}
					}
				}
				for (unsigned i(0); i < nThreads; i++) {
					if (completed[i] == 2) {
						threads[i].join();
						completed[i] = 0;
						completedCount++;
						std::cout << "Thread " << i << " is over!" << std::endl;
					}
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<std::unordered_map<std::string, double>> GridSearch::makeParamSets()
		{
			std::vector<std::unordered_map<std::string, double>> grid;
			grid.push_back(std::unordered_map<std::string, double>());
			for (auto paramVals : paramGrid_) {
				std::vector<std::unordered_map<std::string, double>> nextGrid;
				for (auto paramSet : grid) {
					for (auto val : paramVals.second) {
						auto paramSetCopy = std::unordered_map<std::string, double>(paramSet);
						paramSetCopy[paramVals.first] = val;
						nextGrid.push_back(paramSetCopy);
					}
				}
				grid = nextGrid;
			}
			return grid;
		}
	}
}