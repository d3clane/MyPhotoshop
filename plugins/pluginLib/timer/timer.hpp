#ifndef PLUGINS_PLUGIN_LIB_TIMER_TIMER_HPP
#define PLUGINS_PLUGIN_LIB_TIMER_TIMER_HPP

#include <chrono>
namespace ps
{

class Timer {
public:
    void start();
    long long deltaInMs() const;
    
private:
    std::chrono::steady_clock::time_point start_time;
};

} // namespace ps
#endif // PLUGINS_PLUGIN_LIB_TIMER_TIMER_HPP