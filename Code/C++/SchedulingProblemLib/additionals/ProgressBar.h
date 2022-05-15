#pragma once

#include <iostream>

namespace scheduling_problem 
{
    namespace additionals 
    {
        class ProgressBar
        {
        private:
            unsigned barWidth_, count_, step_;
            float progress_;

        public:

            ProgressBar(unsigned barWidth, 
                        unsigned count);

            void show();

            void reset();
        };
    }
}

