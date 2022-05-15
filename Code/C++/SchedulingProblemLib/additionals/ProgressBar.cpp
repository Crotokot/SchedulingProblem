#include "ProgressBar.h"

namespace scheduling_problem
{
    namespace additionals
    {
        ProgressBar::ProgressBar(unsigned barWidth, 
                                 unsigned count) 
                                 : barWidth_(barWidth)
                                 , count_(count)
                                 , step_(0)
                                 , progress_(0.0)
        {
            
        }

        void ProgressBar::show()
        {
            if (step_ && step_ <= count_) {
                std::cout << "[";
                unsigned pos = (unsigned)(barWidth_ * progress_);
                for (unsigned i(0); i < barWidth_; i++) {
                    if (i < pos) {
                        std::cout << "=";
                    }
                    else if (i == pos) {
                        std::cout << ">";
                    }
                    else {
                        std::cout << " ";
                    }
                }
                std::cout << "] " << unsigned(progress_ * 100.0f) << "%\r";
                std::cout.flush();
            }
            if (step_ == count_) {
                std::cout << std::endl;
            }
            progress_ += 1.f / count_;
            step_++;
        }

        void ProgressBar::reset()
        {
            progress_ = 0.0;
            step_ = 0;
        }
    }
}