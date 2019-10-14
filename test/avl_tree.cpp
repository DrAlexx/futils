
#include <util/binary_tree.h>

#include <string>
#include <iostream>

#define BOOST_TEST_MODULE ShardMapTest
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( AvlTree_CREATE )
{
    BinaryTree<int>  int_set;
    for(int i=1; i < 128 ; ++i) {
        int_set.insert(i);
    }

    std::ostringstream ss;
    int_set.dump_tree(ss);
    std::cout << ss.str() << std::endl;
}



