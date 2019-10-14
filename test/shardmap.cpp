
#include <util/shardmap.h>
#include <util/scoped_profiler.h>

#include <string>
#include <iostream>

#define BOOST_TEST_MODULE ShardMapTest
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( ShardMapTest_CREATE )
{
    using Map = ShardMap<int, int, std::map<int,int>>;
    Map    int_map(2);
    int_map.insert({2, 1});

    using SMap = StringShardMap<int>;
    SMap    str_map(4);
    str_map.insert({"str", 1});
}
