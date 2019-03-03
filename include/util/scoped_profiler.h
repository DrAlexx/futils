#pragma once

#include <atomic>
#include <chrono>
#include <string>
#include <vector>

namespace ScopedProfiler {

using namespace std::chrono;

class Manager;

struct Info
{
    Info(Manager& mgr, const char* str);
    const std::string           name;
    std::atomic_uint_fast64_t   call_count         = 0;
    std::atomic_uint_fast64_t   cumulative_time_us = 0;
};

class Manager
{
public:
    void AddInfo(Info* info) {
        info_array.push_back(info);
    }

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
} //namespace
