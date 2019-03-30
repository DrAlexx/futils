
#include <util/binary_tree.h>

#include <string>
#include <iostream>

#define BOOST_TEST_MODULE ShardMapTest
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( AvlTree_CREATE )
{
    BinaryTree<int, void>    int_set;
    int_set.insert(1);
    int_set.insert(2);
    int_set.insert(10);
    int_set.insert(5);
    int_set.erase(3);
    int_set.erase(2);

    BinaryTree<int, int>    int_map;
    int_map.insert({1,1});
    int_map.insert({2,2});
    int_map.insert({10,10});
    int_map.insert({5,5});
    int_map.erase(3);
    int_map.erase(2);
}
