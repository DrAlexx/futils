
#include <util/binary_tree.h>


#define BOOST_TEST_MODULE Binary_Tree
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Binary_Tree)

BOOST_AUTO_TEST_CASE(AVL_Tree)
{
    using KEY_TYPE = int;
    BinaryTree<KEY_TYPE>  avl_tree;
    constexpr KEY_TYPE FIRST = -1000;
    constexpr KEY_TYPE LAST  = 1000;
    BOOST_REQUIRE(FIRST < LAST);
    BOOST_REQUIRE_EQUAL(avl_tree.size(), 0);
    auto size = avl_tree.size();

    //Insert
    for(auto i = FIRST; i <= LAST; ++i) {
        BOOST_REQUIRE(avl_tree.count(i) == 0);
        BOOST_REQUIRE(avl_tree.insert(i));
        BOOST_REQUIRE(!avl_tree.insert(i));
        BOOST_REQUIRE(avl_tree.count(i) == 1);
        BOOST_REQUIRE_EQUAL(avl_tree.size(), ++size);
        avl_tree.check_height_test([](int hl, int hr){
            BOOST_REQUIRE(std::abs(hl-hr) <= 1);
        });
    }

    //Contains
    BOOST_REQUIRE(avl_tree.count(FIRST - 1) == 0);
    BOOST_REQUIRE(avl_tree.count(LAST  + 1) == 0);
    BOOST_REQUIRE(avl_tree.count(LAST*2) == 0);
    BOOST_REQUIRE(avl_tree.count(FIRST*2) == 0);
    for(auto i = FIRST; i <= LAST ; ++i) {
        BOOST_REQUIRE(avl_tree.count(i) == 1);
    }

    //Erase
    size = avl_tree.size();
    for(auto i = FIRST; i <= LAST; ++i) {
        BOOST_REQUIRE(avl_tree.count(i) == 1);
        BOOST_REQUIRE(avl_tree.erase(i));
        BOOST_REQUIRE(avl_tree.count(i) == 0);
        BOOST_REQUIRE(!avl_tree.erase(i));
        BOOST_REQUIRE_EQUAL(avl_tree.size(), --size);
        avl_tree.check_height_test([](int hl, int hr){
            BOOST_REQUIRE(std::abs(hl-hr) <= 1);
        });
    }
    BOOST_REQUIRE_EQUAL(avl_tree.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
