
#include <util/binary_tree.h>


#define BOOST_TEST_MODULE Binary_Tree
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Binary_Tree)
BOOST_AUTO_TEST_CASE(AVL_Tree_insert)
{
    using KEY_TYPE = int;
    BinaryTree<KEY_TYPE>  avl_tree;
    constexpr KEY_TYPE FIRST = -1234;
    constexpr KEY_TYPE LAST  = 4321;
    BOOST_REQUIRE(FIRST < LAST);
    BOOST_REQUIRE_EQUAL(avl_tree.size(), 0);
    auto size = avl_tree.size();
    for(auto i = FIRST; i <= LAST; ++i) {
        BOOST_REQUIRE(!avl_tree.contains(i));
        BOOST_REQUIRE(avl_tree.insert(i));
        BOOST_REQUIRE(!avl_tree.insert(i));
        BOOST_REQUIRE(avl_tree.contains(i));
        BOOST_REQUIRE_EQUAL(avl_tree.size(), ++size);
        avl_tree.check_height_test([](int hl, int hr){
            BOOST_REQUIRE(std::abs(hl-hr) <= 1);
        });
    }
    BOOST_REQUIRE(!avl_tree.contains(FIRST - 1));
    BOOST_REQUIRE(!avl_tree.contains(LAST  + 1));
    BOOST_REQUIRE(!avl_tree.contains(LAST*2));
    BOOST_REQUIRE(!avl_tree.contains(FIRST*2));
    for(auto i = FIRST; i <= LAST ; ++i) {
        BOOST_REQUIRE(avl_tree.contains(i));
    }
}

BOOST_AUTO_TEST_SUITE_END()
