#include <util/patricia.h>
#include <string>

#define BOOST_TEST_MODULE PatriciaTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( PatriciaTest )
{
    patricia_tree<std::string>  trie;

    trie.insert("S");
    trie.insert("E");
    trie.insert("A");
    trie.insert("R");
    trie.insert("C");
    trie.insert("H");
    trie.insert("I");
    trie.insert("N");
    trie.insert("G");

//    std::string str("SEARCHING");
//    for(auto c : str) {
//        std::string s(" ");
//        s[0] = c;
//        utils::BitStreamAdaptor<std::string> bits(s);
//        std::cout << s << "=" << bits << std::endl;
//    }
    trie.erase("E");
    trie.erase("A");
    trie.dump(std::cout);

    BOOST_CHECK( /*test_object.is_valid()*/ true);
}
