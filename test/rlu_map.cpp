
#include <util/rlu_map.h>


#define BOOST_TEST_MODULE Binary_Tree
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(RLU_Map)

BOOST_AUTO_TEST_CASE(RLU_Map)
{
    rlu_map<int, int>   map(100 * sizeof(int));
}

BOOST_AUTO_TEST_SUITE_END()
