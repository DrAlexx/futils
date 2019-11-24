
#include <util/binary_tree.h>

#include <string>
#include <iostream>

#define BOOST_TEST_MODULE AVL_Tree_Test
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Binary_Tree)
BOOST_AUTO_TEST_CASE( AvlTree_Insert )
{
    BinaryTree<int>  int_set;
    BOOST_REQUIRE_EQUAL(int_set.size(), 0);

    for(int i=1; i < 128 ; ++i) {
        int_set.insert(i);
        BOOST_REQUIRE_EQUAL(int_set.size(), i);

        int_set.check_height_test([](int hl, int hr){
            BOOST_REQUIRE(std::abs(hl-hr) <= 1);
        });
    }
    BOOST_REQUIRE(!int_set.contains(-1));
    BOOST_REQUIRE(!int_set.contains(0));
    BOOST_REQUIRE(!int_set.contains(128));
    for(int i=1; i < 128 ; ++i) {
        BOOST_REQUIRE(int_set.contains(i));
    }
}
BOOST_AUTO_TEST_SUITE_END()
