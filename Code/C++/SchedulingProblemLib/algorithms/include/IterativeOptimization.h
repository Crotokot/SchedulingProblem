#pragma once

#include "algorithms/include/BaseOptimization.h"

namespace scheduling_problem
{
    extern BaseOptimization BASELINE;

    class IterativeOptimization :
        virtual public BaseOptimization
    {
    private:
        enum class Params { SATURATION };
        std::unordered_map<std::string, Params> paramsMap_ = { {"saturation", Params::SATURATION } };

    protected:
        unsigned itersCount_, saturation_;
        std::vector<scheduling_problem::weight_t> descentDynamics_, costDynamics_;
        BaseOptimization baseline_;

    public:
        IterativeOptimization(BaseOptimization& baseline = BASELINE, unsigned saturation = 0);

        unsigned itersCount();

        virtual void setParams(const std::unordered_map<std::string, double>& params);

        virtual Schedule schedule(const Graph& graph) = 0;
    };
}

