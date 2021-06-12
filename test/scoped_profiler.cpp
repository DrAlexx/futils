#include <util/scoped_profiler.h>

#include <algorithm>
#include <thread>
#include <ostream>

#define BOOST_TEST_MODULE Scoped_Profiler
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Scoped_Profiler)

ScopedProfiler::Manager mgr_time;

BOOST_AUTO_TEST_CASE(Time_measurment)
{
    int test_time_us = 1000000; //1s
    int delta_us = 10000; //10ms

    std::cout << "Wait for " << test_time_us << "us" << std::endl;
    std::cout << "delta is " << delta_us << "us" << std::endl;
    {
        static const char point_name[] = "Test point";
        ScopedProfiler::ScopedPoint<mgr_time, point_name>   test_point;

        std::this_thread::sleep_for(std::chrono::microseconds(test_time_us));
    }
    bool has_result = false;
    ScopedProfiler::Manager::get_manager<mgr_time>().for_each_point([&has_result, test_time_us, delta_us](const std::string_view name, uint64_t call_count, uint64_t cumulative_time_us){
        has_result = true;
        BOOST_REQUIRE_EQUAL(name, "Test point");
        BOOST_REQUIRE_EQUAL(call_count, 1);
        BOOST_REQUIRE(((int)cumulative_time_us - test_time_us) < delta_us);
        std::cout << "Measured time is " << cumulative_time_us << "us" << std::endl;
    });
    BOOST_REQUIRE(has_result);
};

ScopedProfiler::Manager mgr_points;

void foo1() {
    static const char point_name[] = "Test point #1";
    ScopedProfiler::ScopedPoint<mgr_points, point_name>   test_point;
}

void foo2() {
    static const char point_name[] = "Test point #2";
    ScopedProfiler::ScopedPoint<mgr_points, point_name>   test_point;
}

BOOST_AUTO_TEST_CASE(Point_count)
{
    {
        static const char point_name[] = "Test point #3";
        ScopedProfiler::ScopedPoint<mgr_points, point_name>   test_point;
    }
    {
        static const char point_name[] = "Test point #4";
        ScopedProfiler::ScopedPoint<mgr_points, point_name>   test_point;
    }
    {
        static const char point_name[] = "Test point #5";
        ScopedProfiler::ScopedPoint<mgr_points, point_name>   test_point;
    }

    foo1();
    foo1();
    foo1();

    struct point {
        std::string name;
        uint64_t call_count;
    };
    std::vector<point> expected_points = {
        point{"Test point #1", 3},
        point{"Test point #2", 0},
        point{"Test point #3", 1},
        point{"Test point #4", 1},
        point{"Test point #5", 1},
    };
    std::vector<point> points;
    points.reserve(expected_points.size());
    ScopedProfiler::Manager::get_manager<mgr_points>().for_each_point([&points](const std::string_view name, uint64_t call_count, uint64_t /*cumulative_time_us*/){
        points.push_back(point{std::string(name), call_count});
    });
    std::sort(points.begin(), points.end(), [](const auto& p1, const auto& p2){
        return p1.name < p2.name;
    });
    BOOST_REQUIRE_EQUAL(points.size(), expected_points.size());
    for(auto i = 0u; i < points.size(); ++i) {
        BOOST_REQUIRE_EQUAL(points[i].name, expected_points[i].name);
        BOOST_REQUIRE_EQUAL(points[i].call_count, expected_points[i].call_count);
    }
}

BOOST_AUTO_TEST_SUITE_END()
