#include "algorithms/include/SimulatedAnnealing.h"
#include "algorithms/include/SUMIACO.h"
#include "algorithms/include/AntQ.h"
#include "experiments/experiments.h"
#include "include/additionals.h"
#include "json/json.hpp"

namespace parse
{
    std::pair<std::string, std::map<std::string, std::vector<std::string>>>
        parseCommand(int argc, char** argv);

    std::string readFile(std::string filename);

    std::unordered_map<std::string, scheduling_problem::BaseOptimization*>
        readOptimizers(const std::vector<std::string>& algs, scheduling_problem::BaseOptimization& baseline);

    std::tuple<std::pair<std::size_t, std::size_t>,
               std::vector<double>,
               std::pair<scheduling_problem::weight_t,
               scheduling_problem::weight_t>>
        readGraphParams(const std::string& path);

    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<double>>>
        readParamGrid(const std::string& path);

    void completeResources(std::unordered_map<std::string, scheduling_problem::BaseOptimization*>& algorithms);
}