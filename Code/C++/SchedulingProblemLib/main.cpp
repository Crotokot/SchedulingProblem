#include <iostream>
#include "include/parse.h"

/*
    Commands:
        * --runs:
            > -alg <alg1.json, alg2.json, ...> - running algorithms from files alg1.json, alg2.json, ... 
              All algorithms can be described in one file. 
            > -pool <path> - read graphs from <path>.
            > -pool <param.json, n_samples, batch_size> - generate graphs with params from <param.json>
            > -dups <n1, n2, ...> - sets list of 'duplicate' values to iterate over.
            > -dups <dups> - read list of 'duplicate' values to iterate over from file <dups>.
            > -output <output_path> - output
        * --test
            > -alg <alg1.json, alg2.json, ...> - running algorithms from files alg1.json, alg2.json, ... 
              All algorithms can be described in one file. 
            > -pool <path> - read graphs from <path>.
            > -pool <param.json> - generate graphs with params from <param.json>
            > -dups n - number of runs for every graph.
            > -output <output_path> - output
        * --gridsearch
            > -alg <alg.json> - optimizer for grid search.
            > -pool <path> - read graphs from <path>.
            > -pool <param.json> - generate graphs with params from <param.json>.
            > -dups n - number of runs for every graph.
            > -grid <grid.json> - parameters grid.
            > -output <output_path> - output
*/


int main(int argc, char** argv)
{
    auto commands = parse::parseCommand(argc, argv);

    scheduling_problem::additionals::DAGGenerator dagGenerator({ 10, 20 }, { 0.2 }, { 10, 50 }, 10, 10, 42); // plug
    scheduling_problem::additionals::DAGPool* dagPool = &dagGenerator;
    scheduling_problem::GreedyOptimization greedy;
    std::unordered_map<std::string, scheduling_problem::BaseOptimization*> algorithms;
    std::vector<unsigned> dupsList;
    std::string output;

    for (auto param : commands.second) {
        if (param.first == "-alg") {
            algorithms = parse::readOptimizers(param.second, greedy);
        }
        else if (param.first == "-pool") {
            auto path = param.second[0];
            unsigned nSamples = std::stoi(param.second[1]), 
                batchSize = std::stoi(param.second[2]);
            if (path.substr(path.size() - 5, path.size()) == ".json") {
                // read graph params from json
                unsigned seed = 42;
                auto graphParams = parse::readGraphParams(path);
                dagPool = new scheduling_problem::additionals::DAGGenerator(std::get<0>(graphParams), 
                                                                            std::get<1>(graphParams),
                                                                            std::get<2>(graphParams), 
                                                                            nSamples, 
                                                                            batchSize, 
                                                                            seed);
            }
            else {
                dagPool = new scheduling_problem::additionals::DAGReader(path, nSamples, batchSize);
            }
        }
        else if (param.first == "-dups") {
            bool isdigit = true;
            for (auto ch : param.second[0]) {
                isdigit &= (bool)std::isdigit(ch);
            }
            if (isdigit) {
                for (auto dup : param.second)
                    dupsList.push_back(std::stoi(dup));
            }
            else {
                unsigned dup;
                std::ifstream file(param.second[0]);
                while (!file.eof()) {
                    file >> dup;
                    dupsList.push_back(dup);
                }
            }
        }
        else if (param.first == "-output") {
            output = param.second[0];
        }
    }

    if (commands.first == "--runs") {
        scheduling_problem::experiments::runsNumberSelection(algorithms, dagPool, dupsList, output);
    }
    else if (commands.first == "--test") {
        scheduling_problem::experiments::algorithmsTesting(algorithms, dagPool, dupsList[0], output);
    }
    else if (commands.first == "--gridsearch") {
        auto paramsPath = commands.second["-grid"][0];
        // read params from json
        auto paramGrids = parse::readParamGrid(paramsPath);
        for (auto optimizer : algorithms) {
            std::cout << optimizer.first << std::endl;
            scheduling_problem::experiments::gridSearch(optimizer.second,
                paramGrids[optimizer.first], dagPool, dupsList[0], output, 2);
            break;
        }
    }

    parse::completeResources(algorithms);
    delete dagPool;
}