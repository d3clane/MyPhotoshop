#include "timer.hpp"

namespace ps
{
    
void Timer::start()
{
    start_time = std::chrono::steady_clock::now();
}

long long Timer::deltaInMs() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start_time).count();
}

} // namespace ps