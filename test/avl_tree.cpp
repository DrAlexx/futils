
#include <binary_tree/binary_tree.h>


#define BOOST_TEST_MODULE Binary_Tree
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Binary_Tree)

BOOST_AUTO_TEST_CASE(AVL_Tree)
BOOST_AUTO_TEST_CASE(AVL_Tree_Max_height)
{
    using KEY_TYPE = int;
    binary_tree::tree<KEY_TYPE>  avl_tree;
    /**
     * node count = 54
     * max height = 7 (log2(54) = 5,75; need stack with max size 7)
     *
     */
    std::vector<int> d{33,
                       20,46,
                       12,28,41,51,
                       07,17,25,31,38,44,49,53,
                       04,10,15,19,23,27,30,32,36,40,43,45,48,50,52,
                       2,6,9,11,14,16,18,22,24,26,29,35,37,39,42,47,
                       1,3,5,8,13,21,34,
                       0};
    for (auto i: d)
        avl_tree.insert(i);
    avl_tree.clear();
}

{
    using KEY_TYPE = int;
    binary_tree::tree<KEY_TYPE>  avl_tree;
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

BOOST_AUTO_TEST_CASE(AVL_Tree_enum) {
    using KEY_TYPE = int;
    binary_tree::tree<KEY_TYPE>  avl_tree;
    constexpr KEY_TYPE FIRST = 0;
    constexpr KEY_TYPE LAST  = 10;
    BOOST_REQUIRE(FIRST < LAST);
    BOOST_REQUIRE_EQUAL(avl_tree.size(), 0);

    for (int i = FIRST; i <= LAST; ++i) {
        avl_tree.insert(i);
    }

   // avl_tree.dump_tree(std::cout);
    avl_tree.enumerate([](auto& v){
        std::cout << v << std::endl;
        return true;
    }, binary_tree::tree<KEY_TYPE>::EnumerationOrder::DESCENDING);
}

BOOST_AUTO_TEST_SUITE_END()
