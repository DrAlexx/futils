#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include <vector>

/**
 * ScopedProfiler is a small header-only library for measuring execution time of your program elements.
 * The library is required C++17 compiler and C++ standard library only. Time measurment is based on std::high_resolution_clock.
 * Every measurment point @see ScopedPoint has a uniqe name and two values:
 *  - call_cout is a number of calls for this point
 *  - cumulative_time_us is a sum of execution time for all calls.
 * You can get average exection time per a call via the equation cumulative_time_us/call_cout.
 *
 * How to use:
 * Create ScopedProfiler::Manager instance. This in a collection of mesurment points.
 * Create ScopedProfiler::ScopedPoint instance in the scope where you want to measure time.
 * ScopedProfiler::ScopedPoint is an template class that have to be specialized with the help of m
 * ScopedProfiler::Manager instance and point name.
 * @note ScopedProfiler::Manager instance have to be global.
 * @note point name must be a static const char.
 * @note also point name should be unique inside manager instance in oherwise different point shoul share one data record inside a manager.
 *
 * Multithreading is supported with some exceptions:
 *  - Any measurment point @see ScopedPoint is thread safe and hasn't got any kind of locks inside.
 *    All synchronization is based on atomics.
 *  - Reading measurments @see Manager::for_each_point info hasn't got any locks but can read non-consistent data.
 *    It is possible when call_count is already updated and cumulative_time_us isn't yet. I won't to fix it since strict synhronization kills perfomance :)
 *
 * @example
 * #include <cout>
 * #include <scoped_profiler.h>
 * using namespace ScopedProfiler;
 * Manager mgr;
 *
 * void foo() {
 *  static const char point_name[] = "Function foo";
 *  ScopedPoint<mgr, point_name>    my_point;
 *
 *  //do something here
 * };
 *
 * int main(int argc, char** argv) {
 *  {
 *      static const char point_name[] = "anon scope in main";
 *      ScopedPoint<mgr, point_name>    my_point;
 *
 *      //do something;
 *  }
 *  foo();
 *
 *  //print results
 *  mgr.for_each_point([](const char* name, uint64_t call_count, uint64_t cumulative_time_us){
 *      std::cout << "Point: " << name << "count: " << call_count << "cumulative_time: " << cumulative_time_us << "us" << std::endl;
 *  });
 *  return 0;
 * }
 */

namespace ScopedProfiler {

using namespace std::chrono;

class Manager;

/**
 * @brief The Info struct contains a measurment named data.
 */
struct Info
{
    /**
     * @brief Info Initialize an instance and stores record pointer into given manager by given
     * Isn't a sence to have an Info instance that isn't registared in a Manager.
     * @param mgr Manager for this info instance
     * @param name is uniq name for this Info instance
     */
    Info(Manager& mgr, const char* name);
    const std::string           name;
    std::atomic_uint_fast64_t   call_count         = 0;
    std::atomic_uint_fast64_t   cumulative_time_us = 0;
};

/**
 * @brief The Manager class is a collection of  @see ScopedPoint.
 * @note The Manager instance must be a global variable.
 */
class Manager
{
public:
    /**
     * @brief AddInfo stores Info struct address inside a manager.
     *  The Info struct instance is placed outside in a ScopedPoint instance
     * @param info
     */
    void AddInfo(Info* info) {
        info_array.push_back(info);
    }

    /**
     * for_each_point is an enumerator that calls functor f for every registered Info record.
     * It can be used for retreiving measured data.
     */
    template<typename F>
    void for_each_point(F f) {
        for(const auto* info : info_array) {
            f(info->name, info->call_count.load(), info->cumulative_time_us.load());
        }
    }
private:
    std::vector<Info*> info_array;
};

Info::Info(Manager& mgr, const char* str)
    :name(str){
    mgr.AddInfo(this);
}

/**
 * ScopedPoint class is one time measurment point.
 * You do no not need start/stop methods since start is an creation of the ScopedPoint
 * and stop is an destruction of the ScopedPoint instance.
 * The class has two template parameters:
 * mgr  - is global variable of Manager type since every point have to be restered in some manager.
 * name - is a unique point name. @note it have to be static const char since template limitation.
 */
template <Manager& mgr, const char* name>
class ScopedPoint {
public:
    ScopedPoint() {
        start = high_resolution_clock::now();
    }
    ~ScopedPoint(){
        auto end = high_resolution_clock::now();
        info.cumulative_time_us += std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        info.call_count++;
    }
private:
    high_resolution_clock::time_point start;
    static inline Info info = Info(mgr, name);
};

} //namespace ScopedProfiler
