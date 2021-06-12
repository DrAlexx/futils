#pragma once

#include <atomic>
#include <chrono>
#include <string_view>
#include <vector>

/** @defgroup ScopedProfiler
 * @brief This is a small one-header library for measuring of the execution time of your program elements.
 *
 * The library is required C++17 compiler and C++ standard library only. The time measurement is based on std::high_resolution_clock.
 * Every measurement point has an unique name and two values:
 *  - call_count is a number of calls for this point
 *  - cumulative_time_us is a sum of execution time for all calls in us.
 * You can get an average execution time per a call via the equation cumulative_time_us/call_count.
 *
 * How to use:
 * Create Manager instance. This is a collection of measurement points.
 * Create ScopedPoint instance in the scope where you want to measure time.
 * ScopedPoint is an template class that have to be specialized with the help of Manager instance and point name.
 * @note Manager instance must be global.
 * @note point name must be a 'static const char'. Also point name must be unique inside manager instance.
 *
 * Multithreading is supported with some exceptions:
 *  - Any measurement point ScopedPoint is thread safe and hasn't got any kind of locks inside.
 *    All synchronization is based on atomics, so the time measurement doesn't take a lot of time.
 *  - Manager::for_each_point can be called in any time but when some point is updating his own state it can provide the inconsistent data.
 *    (but no crash or memory corruption here)
 *    It will be a good idea to divide the time measurement and data statistic processing. But it's not a strict requirement.
 *  - Manager::reset is completely not thread-safe. If you definitely want to reset all data you must ensure that any points don't change the data in the same time.
 *    (but no crash or memory corruption here)
 *
 * Example:
 * @code
 * #include <scoped_profiler.h>
 * #include <ostream>
 *
 * ScopedProfiler::Manager mgr;
 *
 * void foo() {
 *     static const char point_name[] = "Function foo";
 *     ScopedProfiler::ScopedPoint<mgr, point_name>    my_foo_point;
 *
 *     //do something here
 * };
 *
 * int main(int argc, char** argv) {
 *     {
 *         static const char point_name[] = "anon scope in main";
 *         ScopedProfiler::ScopedPoint<mgr, point_name>    my_anon_point;
 *
 *         //do something;
 *
 *     }
 *
 *     //print results
 *     mgr.for_each_point([](const char* name, uint64_t call_count, uint64_t cumulative_time_us){
 *         std::cout << "Point: " << name << "count: " << call_count << "cumulative_time: " << cumulative_time_us << "us" << std::endl;
 *     });
 *     return 0;
 * }
 * @endcode
 */

namespace profiler {

struct point_set;

struct base_info {
    base_info(const char* str)
        : name(str)
    {}
    const std::string_view      name;
    std::atomic_uint_fast64_t   call_count         = 0;
    std::atomic_uint_fast64_t   cumulative_time_us = 0;
};


/**
 * @private Info
 * @brief The Info struct contains the measurement named data.
 * You don't need to use it directly
 */
template <point_set& mgr>
struct point_info : public base_info {
    /**
     * @brief Info initializes an instance and stores Info pointer into the given manager by the given name
     * There is no point to have the Info instance which is not registered in the Manager.
     * @arg mgr Manager for this info instance
     * @arg name is unique name for this Info instance
     */
    point_info(const char* str);
};

class point_set_impl {
public:
    point_set_impl(const point_set_impl&) = delete;
    point_set_impl(const point_set_impl&&) = delete;
    point_set_impl& operator=(const point_set_impl&) = delete;
    point_set_impl(){}

    /**
     * @brief for_each_point is an enumerator that calls the functor f for every ScopedPoint which is linked to the Manager.
     *
     * This method can be called in any time but when some point is updating his own state it can provide the inconsistent data.
     * (but no crash or memory corruption here)
     * @tparam F callable object that has the next signature foo(const std::string_view name, uint64_t call_count, uint64_t cumulative_time_us)
     * @arg f this callable object will be called for every ScopedPoint registered in the Manager.
     */
    template<typename F>
    void for_each_point(F f) {
        for(const auto* info : info_array) {
            f(info->name, info->call_count.load(), info->cumulative_time_us.load());
        }
    }

    /**
     * @brief reset cleans all call counters and cumulative time counters.
     *
     * This method is completely not thread-safe. If you definitely want to reset all data you must ensure that any points don't change the data in the same time.
     * (but no crash or memory corruption here)
     */
    void reset() {
        for(auto* info : info_array) {
            info->call_count = 0;
            info->cumulative_time_us = 0;
        }
    }

    std::vector<base_info*> info_array;

    /**
     * @private
     * @brief AddInfo stores the Info struct address inside the manager.
     *
     *  The Info struct instance is placed outside in a ScopedPoint instance.
     *  You don't need to call it directly
     * @arg info
     */
    void AddInfo(base_info* info) {
        info_array.push_back(info);
    }
};

/** @ingroup ScopedProfiler ScopedProfiler
 * @class point_set
 * @brief The point_set class is a collection of point`s.
 * @note The point_set instance must be a global variable.
 */
struct point_set {
    point_set(const point_set&) = delete;
    point_set(const point_set&&) = delete;
    point_set& operator=(const point_set&) = delete;
    point_set(){}

    template <point_set& mgr>
    static point_set_impl& get_manager() {
        static point_set_impl manager_impl;
        return manager_impl;
    }
};

template <point_set& mgr>
point_info<mgr>::point_info(const char* str)
    : base_info (str) {
    point_set::get_manager<mgr>().AddInfo(this);
}

/** @ingroup ScopedProfiler
 * @class point
 * @brief It is a time measurement point.
 *
 * You don't need start/stop methods since start is an creation of the point
 * and stop is an destruction of the point instance.
 * The class has two template parameters:
 * @tparam mgr  - is global variable of Manager type since every point have to be registered in some manager.
 * @tparam name - is a unique point name.
 * @note name must be static const char* since template limitation.
 */
template <point_set& mgr, const char* name>
class point {
private:
    using clock = std::chrono::high_resolution_clock;

public:
    point() {
        start = clock::now();
    }
    ~point(){
        auto end = clock::now();
        info.cumulative_time_us += std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        info.call_count++;
    }

private:
    clock::time_point start;
    static inline point_info<mgr> info = point_info<mgr>(name);
};

} //namespace ScopedProfiler
