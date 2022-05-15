#include "include/parse.h"

namespace parse
{
    std::pair<std::string, std::map<std::string, std::vector<std::string>>>
        parseCommand(int argc, char** argv)
    {
        std::pair<std::string, std::map<std::string, std::vector<std::string>>>
            command("", std::map<std::string, std::vector<std::string>>());
        std::vector<std::string> params;
        if (argc <= 1) {
            char ch;
            std::string word = "";
            while ((ch = getchar()) != '\n' && ch != EOF) {
                if (ch == ' ') {
                    params.push_back(word);
                    word = "";
                }
                else {
                    word += ch;
                }
            }
            params.push_back(word);
        }
        else {
            for (int i(1); i < argc; i++) {
                params.push_back(argv[i]);
            }
        }

        std::string lastKey;
        for (auto word : params) {
            if (word[0] == word[1] && word[0] == '-') {
                command.first = word;
            }
            else if (word[0] == '-') {
                command.second[word] = std::vector<std::string>();
                lastKey = word;
            }
            else {
                command.second[lastKey].push_back(word);
            }
        }
        return command;
    }

    std::string readFile(std::string filename)
    {
        std::ifstream file(filename, std::ios::binary);
        int size = (int)file.seekg(0, std::ios::end).tellg();
        file.seekg(0);
        char* str = new char[size + 1];
        file.read(str, size);
        str[size] = '\0';
        std::string s(str);
        delete[] str;
        return s;
    }


    std::unordered_map<std::string, scheduling_problem::BaseOptimization*>
        readOptimizers(const std::vector<std::string>& algs, scheduling_problem::BaseOptimization& baseline)
    {
        std::unordered_map<std::string, scheduling_problem::BaseOptimization*> algorithms;
        for (auto algJson : algs) {
            auto json = nlohmann::json::parse(readFile(algJson));
            for (auto alg : json.items()) {
                auto params = alg.value();
                if (alg.key() == "ACO") {
                    algorithms["ACO"] =
                        new scheduling_problem::AntColonyOptimization(params["pheDecay"],
                                                                      params["threshold"],
                                                                      params["pheInfluence"],
                                                                      params["heuInfluence"],
                                                                      params["epochs"],
                                                                      params["ants"],
                                                                      baseline,
                                                                      params["saturation"],
                                                                      params["randomState"]);
                }
                else if (alg.key() == "SAO") {
                    algorithms["SAO"] =
                        new scheduling_problem::SimulatedAnnealing(params["minTemp"],
                                                                   params["maxTemp"],
                                                                   params["reductionRule"],
                                                                   baseline,
                                                                   params["saturation"],
                                                                   params["randomState"]);
                }
                else if (alg.key() == "ACOLS") {
                    algorithms["ACOLS"] =
                        new scheduling_problem::SUMIACO(params["pheDecay"],
                                                        params["threshold"],
                                                        params["pheInfluence"],
                                                        params["heuInfluence"],
                                                        params["epochs"],
                                                        params["ants"],
                                                        baseline,
                                                        params["saturation"],
                                                        params["randomState"],
                                                        params["localSearchIters"]);
                }
                else if (alg.key() == "AntQ") {
                    algorithms["AntQ"] =
                        new scheduling_problem::AntQ(params["pheDecay"],
                                                     params["threshold"],
                                                     params["pheInfluence"],
                                                     params["heuInfluence"],
                                                     params["gamma"],
                                                     params["W"],
                                                     params["epochs"],
                                                     params["ants"],
                                                     baseline,
                                                     params["saturation"],
                                                     params["randomState"]);
                }
            }
        }
        return algorithms;
    }

    std::tuple<std::pair<std::size_t, std::size_t>,
        std::vector<double>,
        std::pair<scheduling_problem::weight_t,
        scheduling_problem::weight_t>>
        readGraphParams(const std::string& path)
    {
        std::pair<std::size_t, std::size_t> nVertices;
        std::vector<double> density;
        std::pair<scheduling_problem::weight_t, scheduling_problem::weight_t> weights;
        auto json = nlohmann::json::parse(readFile(path));
        for (auto item : json.items()) {
            auto values = item.value();
            if (item.key() == "nVertices") {
                nVertices = std::make_pair(values[0], values[1]);
            }
            else if (item.key() == "density") {
                for (auto val : values) {
                    density.push_back(val);
                }
            }
            else if (item.key() == "weights") {
                weights = std::make_pair((scheduling_problem::weight_t)values[0],
                    (scheduling_problem::weight_t)values[1]);
            }
        }
        return std::make_tuple(nVertices, density, weights);
    }


    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<double>>>
        readParamGrid(const std::string& path)
    {
        std::unordered_map<std::string, std::unordered_map<std::string, std::vector<double>>> paramGrids;
        auto json = nlohmann::json::parse(readFile(path));
        for (auto item : json.items()) {
            paramGrids[item.key()] = std::unordered_map<std::string, std::vector<double>>();
            for (auto params : item.value().items()) {
                paramGrids[item.key()][params.key()] = std::vector<double>();
                for (auto val : params.value()) {
                    paramGrids[item.key()][params.key()].push_back(val);
                }
            }
        }
        return paramGrids;
    }

    void completeResources(std::unordered_map<std::string, scheduling_problem::BaseOptimization*>& algorithms)
    {
        for (auto optimizer : algorithms) {
            if (optimizer.first == "ACO") {
                delete dynamic_cast<scheduling_problem::AntColonyOptimization*>(optimizer.second);
            }
            else if (optimizer.first == "SAO") {
                delete dynamic_cast<scheduling_problem::SimulatedAnnealing*>(optimizer.second);
            }
            else if (optimizer.first == "ACOLS") {
                delete dynamic_cast<scheduling_problem::SUMIACO*>(optimizer.second);
            }
            else if (optimizer.first == "AntQ") {
                delete dynamic_cast<scheduling_problem::AntQ*>(optimizer.second);
            }
        }
    }

}
