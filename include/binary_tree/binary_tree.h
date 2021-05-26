#pragma once

#include "avl_balancer.h"

#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <sstream>

/** @defgroup binary_tree binary_tree
 * @brief Self balanced binary tree (AVL, RB, etc)
 *
 * The binary tree is a famous data structure that has been described many times.
 * As example you can find the description here <a href="https://en.wikipedia.org/wiki/Binary_tree">Wiki - Binary tree</a>.
 *
 * Guess you know such standart C++ classes as <a href="https://en.cppreference.com/w/cpp/container/set">std::set</a>
 * and <a href="https://en.cppreference.com/w/cpp/container/map">std::map</a>. They are ussualy implemented as
 * <a href="https://en.wikipedia.org/wiki/Red%E2%80%93black_tree">Wiki - Red-black tree</a>.
 * This implementaions is similar to STL but have some differences:
 *  - Here is only one class namely binary_tree that is a map and the set.
 *    if you want to have a set you should pass template parameter T as void.
 *    as example:
 *    @code
 *    binary_tree::binary_tree<int, void> my_set; //this is a set that can store int keys only
 *
 *    binary_tree::binary_tree<int, std::string> my_map; //this is a map where key has type int and maped value is std::string
 *    @endcode
 *  - This implementaion uses only two links per node. It helps to reduce memory usage and I hope it helps to improve performance.
 *    But without a link to the parent node impossible to implement iterator based acces. So it isn't present.
 *    Instead you can use enumerators.
 *  - balancer type can be customized via template parameter B. But now only avl_balancer is implemented.
 *
 *  avl_balancer provides <a href="https://en.wikipedia.org/wiki/AVL_tree">AVL tree</a>.
 *  Most implementations of AVL tree are recursive. But this library contains iterative inserting and removing.
 */

namespace binary_tree {

/** @ingroup binary_tree
 * @class tree
 * @brief This is a binary tree that can be cus
 *
 * Keys are sorted by using the comparison function Compare.
 * Search, removal, and insertion operations have logarithmic complexity.
 * @tparam Key Type of the keys. Each element in a tree is uniquely identified by its key value.
 *         Aliased as member type tree::key_type.
 * @tparam T Type of the mapped value. Each element in a tree stores some data as its mapped value.
 *         Aliased as member type tree::mapped_type. By the default this type is void.
 * @tparam B balancer type. Now ypu can use only binary_tree::avl_balancer here!
 * @tparam Compare A binary predicate that takes two element keys as arguments and returns a bool.
 * The expression comp(a,b), where comp is an object of this type and a and b are key values,
 * shall return true if a is considered to go before b in the strict weak ordering the function defines.
 * The map object uses this expression to determine both the order the elements follow in the container
 * and whether two element keys are equivalent (by comparing them reflexively: they are equivalent if !comp(a,b) && !comp(b,a)).
 * No two elements in a map container can have equivalent keys.
 * This can be a function pointer or a function object (see constructor for an example).
 * This defaults to less<T>, which returns the same as applying the less-than operator (a<b). Aliased as member type tree::key_compare.
 * @tparam Alloc Type of the allocator object used to define the storage allocation model.
 * By default, the allocator class template is used, which defines the simplest memory allocation model and is value-independent.
 */
template<typename Key, typename T = void, typename B = binary_tree::avl_balancer, typename Compare = std::less<Key>,
         template<typename X> typename Alloc = std::allocator>
class tree
{
private:
    struct Node  : public B::Node {
        using key_type      = Key;
        using mapped_type   = T;

        using value_type = typename std::conditional<
            std::is_void<mapped_type>::value,
            key_type,
            std::pair<key_type, mapped_type>>::type;

        Node* links[2] = {nullptr, nullptr};
        value_type  value;

        Node(const value_type& v)
            : value(v)
        {}

        template<typename K>
        int get_direction(const K& key) {
            return key_compare{}(key, value)? 0 : 1;
        }
        template<typename K>
        auto get_next(const K& key) {
            return links[get_direction(key)];
        }

        static const key_type& get_key_impl(const value_type& v, std::true_type) {
            return v;
        }
        static const key_type& get_key_impl(const value_type& v, std::false_type) {
            return v.first;
        }
        static const key_type& get_key(const value_type& v) {
            return get_key_impl(v, std::is_void<mapped_type>());
        }

        //For test/debug purpose
        static int get_height(Node* n) {
            if (n == nullptr)
                return 0;
            return std::max(get_height(n->links[0]), get_height(n->links[1]))+1;
        }
    };

    using node_type       = Node;
    using node_pointer    = node_type*;
    using Node_alloc_type = Alloc<node_type>;

public:
    ///Alias for Key
    using key_type       = Node::key_type;
    ///Alias for T
    using mapped_type    = Node::mapped_type;
    /**
     * When T isn't void value_type is std::pair<Key,T>.
     * In the other case it is alias for Key
     */
    using value_type     = Node::value_type;
    /// Alias for Compare
    using key_compare    = Compare;

    using size_type = std::size_t;

    //Construct
    /**
     * @brief tree creates empty tree
     * TODO: impl
     */
    tree() noexcept;

    /**
     * @brief tree not impl yet
     * TODO: impl
     */
    tree(tree&) = delete;

    /**
     * @brief tree not impl yet
     * TODO: impl
     */
    tree(tree&&) = delete;

    /**
     * @brief ~tree destructor
     */
    ~tree();

    ///TODO: impl
    tree& operator=(const tree& other) = delete;
    ///TODO: impl
    tree& operator=(tree&& other) = delete;
    ///TODO: impl
    tree& operator=(std::initializer_list<value_type> ilist) = delete;

    //Capacity
    /**
     * @brief empty
     * @return true if tree is empty otherwise false
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief size
     * @return the number of elements in the container
     */
    size_type size() const noexcept;

    /**
     * @brief max_size
     * @return the maximum possible number of elements
     */
    size_type max_size() const noexcept;

    //Modifiers
    /**
     * @brief Inserts element into the container, if the container doesn't already contain an element with an equivalent key.
     * @param value element value to insert
     * @return true when an element was inserted otherwise false
     */
    bool insert(const value_type& value);

    /**
     * @brief erase Removes specified elements from the container.
     * @param key
     * @return Number of elements removed.
     */
    size_type erase(const key_type& key);

    /**
     * @brief swap Exchanges the contents of the container with those of other.
     *
     * Does not invoke any move, copy, or swap operations on individual elements.
     * @param other container to exchange the contents with
     */
    void swap(tree<Key, T, B, Compare, Alloc>& other);

    /**
     * @brief clear Erases all elements from the container.
     *
     * After this call, size() returns zero.
     */
    void clear() noexcept;
//    template <class... Args>
//    bool emplace (Args&&... args);

    //Lookup
    /**
     * @brief count Returns the number of elements with key that compares equivalent to the specified argument.
     * @param key value of the elements to count
     * @return Number of elements with key that compares equivalent to key or x, which is either 1 or 0 for (1).
     */
    template <typename K>
    size_type count(const K& x) const;

    template <typename K>
    bool contains(const K& x) const {
        return count(x) != 0;
    }

    /**
     * @brief calls functor f for every element in the tree
     * @tparam F functional object type
     */
    template <typename F>
    void enumerate(F f);

    template <typename K, typename F>
    void enumerate_lower_bound(const K& x, F f);

    template <typename K, typename F>
    void enumerate_upper_bound(const K& x, F f);

    //Test & debug
    /**
     * @brief check_height_test for testing purposes only!
     */
    template<typename F>
    void check_height_test(F check_height) const {
        recursive_check_height(root, check_height);
    }

    /**
     * @brief dump_tree dumps tree content into graphviz BST format
     * @param ss
     */
    void dump_tree (std::ostringstream& ss) const {
        ss << "digraph BST {\n";
        recursive_dump(root, ss);
        ss << "}\n";
    }

private:
    Node_alloc_type  node_allocator;
    node_pointer     root = nullptr;
    size_t           node_count = 0;

    template <typename F>
    void recursive_enumerate(node_pointer start_node, F f);

    template<typename K>
    static node_pointer lookup(node_pointer node, const K& key) {
        while (node != nullptr && key != Node::get_key(node->value))
            node = node->get_next(key);
        return node;
    }

    //For testing
    template<typename F>
    static void recursive_check_height(node_pointer node, F check_height) {
        if (node == nullptr) return;
        recursive_check_height(node->links[0], check_height);
        recursive_check_height(node->links[1], check_height);
        check_height(node_type::get_height(node->links[0]), node_type::get_height(node->links[1]));
    }

    static void recursive_dump(node_pointer node, std::ostringstream& ss) {
        if (node != nullptr) {
            ss << '"' << get_key(node->value) << "\" -> { " ;
            if (node->links[0] != nullptr) {
                ss << '"' << get_key(node->links[0]->value) << "\" ";
            }
            if (node->links[1] != nullptr) {
                ss << '"' << get_key(node->links[1]->value) << "\" ";
            }
            ss << "}\n";
            recursive_dump(node->links[0], ss);
            recursive_dump(node->links[1], ss);
        }
    }
};

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
tree<Key, T, B, Compare, Alloc>::tree() noexcept
{}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
tree<Key, T, B, Compare, Alloc>::~tree() {
    clear();
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
bool tree<Key, T, B, Compare, Alloc>::empty() const noexcept {
    return size() == 0;
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
tree<Key, T, B, Compare, Alloc>::size_type tree<Key, T, B, Compare, Alloc>::size() const noexcept {
    return node_count;
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
tree<Key, T, B, Compare, Alloc>::size_type tree<Key, T, B, Compare, Alloc>::max_size() const noexcept {
    return std::numeric_limits<size_t>::max()/sizeof(tree<Key, T, B, Compare, Alloc>::node_type);
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
bool tree<Key, T, B, Compare, Alloc>::insert(const tree<Key, T, B, Compare, Alloc>::value_type& value) {
    return B::insert(&root, value, [this](tree<Key, T, B, Compare, Alloc>::Node** parent_ptr, const tree<Key, T, B, Compare, Alloc>::value_type& value){
        auto new_node = node_allocator.allocate(1);
        std::allocator_traits<Node_alloc_type>::construct(node_allocator, new_node, value);
        ++node_count;
        *parent_ptr = new_node;
    });
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
tree<Key, T, B, Compare, Alloc>::size_type tree<Key, T, B, Compare, Alloc>::erase(const tree<Key, T, B, Compare, Alloc>::key_type& key) {
    auto targetn = B::erase(&root, key);
    if (targetn != nullptr) {
        std::allocator_traits<Node_alloc_type>::destroy(node_allocator, targetn);
        node_allocator.deallocate(targetn, 1);
        --node_count;
        return 1;
    }
    return 0;
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
template<typename K>
tree<Key, T, B, Compare, Alloc>::size_type tree<Key, T, B, Compare, Alloc>::count(const K& x) const
{
    return tree<Key, T, B, Compare, Alloc>::lookup(root, x) == nullptr? 0 : 1;
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
template <typename F>
void tree<Key, T, B, Compare, Alloc>::enumerate(F f) {
    //FIXME: not impl
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
void tree<Key, T, B, Compare, Alloc>::swap(tree<Key, T, B, Compare, Alloc>& other) {
    std::swap(node_count, other.node_count);
    std::swap(root, other.root);
    std::swap(node_allocator, other.node_allocator);
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
void tree<Key, T, B, Compare, Alloc>::clear() noexcept
{
    recursive_enumerate(root, [this](auto* node){
        std::allocator_traits<Node_alloc_type>::destroy(node_allocator, node);
        node_allocator.deallocate(node, 1);
    });

    root = nullptr;
    node_count = 0;
}

template <typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
template <typename F>
void tree<Key, T, B, Compare, Alloc>::recursive_enumerate(node_pointer start_node, F f) {
    if(start_node == nullptr)
        return;

    if(start_node->links[0] != nullptr)
        recursive_enumerate(start_node->links[0], f);

    if(start_node->links[1] != nullptr)
        recursive_enumerate(start_node->links[1], f);

    f(start_node);
}
}
