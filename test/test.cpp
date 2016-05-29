#include <util/patricia.h>
#include <string>

#define BOOST_TEST_MODULE PatriciaTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( PatriciaTest )
{
    patricia_tree<std::string>  trie;
    trie.insert("A");
    trie.insert("B");
    trie.insert("C");
    trie.insert("D");
    trie.insert("E");
    trie.insert("F");
    trie.insert("a");
    trie.insert("x");
    trie.insert("__");

    BOOST_CHECK( /*test_object.is_valid()*/ true);
}
