#pragma once

#include "avl_balancer.h"

#include <functional>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include <sstream>

namespace binary_tree {

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

        int get_direction(const key_type& key) {
            return Compare{}(key, value)? 0 : 1;
        }
        auto get_next(const key_type& key) {
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
    using key_type    = Node::key_type;
    using mapped_type = Node::mapped_type;
    using value_type  = Node::value_type;

    //Construct
    tree() noexcept;
    tree(tree&) = delete;
    tree(tree&&) = delete;
    ~tree();

    //Capacity
    bool empty() const noexcept;
    size_t size() const noexcept;
    size_t max_size() const noexcept;

    //Modifiers
    bool insert(const value_type& value) {
        return B::insert(&root, value, [this](Node** parent_ptr, const value_type& value){
            auto new_node = node_allocator.allocate(1);
            std::allocator_traits<Node_alloc_type>::construct(node_allocator, new_node, value);
            ++node_count;
            *parent_ptr = new_node;
        });
    }
    size_t erase(const key_type& key) noexcept {
        auto targetn = B::erase(&root, key);
        if (targetn != nullptr) {
            std::allocator_traits<Node_alloc_type>::destroy(node_allocator, targetn);
            node_allocator.deallocate(targetn, 1);
            --node_count;
            return 1;
        }
        return 0;
    }
    void swap(tree<Key, T, B, Compare, Alloc>& other);
    void clear();
//    template <class... Args>
//    bool emplace (Args&&... args);

    //Operations
    size_t count(const key_type& key) const;
    template <typename F>
    void enumerate(F f);

    //Test & debug
    template<typename F>
    void check_height_test(F check_height) const {
        recursive_check_height(root, check_height);
    }
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

    static node_pointer lookup(node_pointer node, const key_type& key) {
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
size_t tree<Key, T, B, Compare, Alloc>::size() const noexcept {
    return node_count;
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
size_t tree<Key, T, B, Compare, Alloc>::max_size() const noexcept {
    return std::numeric_limits<size_t>::max()/sizeof(tree<Key, T, B, Compare, Alloc>::node_type);
}

template<typename Key, typename T, typename B, typename Compare, template<typename X> typename Alloc>
size_t tree<Key, T, B, Compare, Alloc>::count(const tree<Key, T, B, Compare, Alloc>::key_type& key) const
{
    return tree<Key, T, B, Compare, Alloc>::lookup(root, key) == nullptr? 0 : 1;
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
void tree<Key, T, B, Compare, Alloc>::clear()
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
