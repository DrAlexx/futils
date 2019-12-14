#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <sstream>

#include "avl_tree_balancer.h"

template<typename Key, typename T = void, typename B = AVLTreeBalancer, template<typename X> typename Alloc = std::allocator>
class BinaryTree
{
public:
    using key_type      = Key;
    using mapped_type   = T;
    using balancer_type = B;

    using value_type = typename std::conditional<
        std::is_void<mapped_type>::value,
        key_type,
        std::pair<key_type, mapped_type>>::type;

    //Construct
    BinaryTree();
    ~BinaryTree();

    //Modifiers
    bool insert(const value_type& value);
//    void erase(const key_type& key);
    void clear();

    //Const
    bool contains(const key_type& key) const;
    size_t size() const;

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
    struct Node : public balancer_type::NodeInfo {
        value_type  value;
        Node*       links[2] = {nullptr, nullptr};
        Node(const value_type& v)
            : value(v)
        {}

        //For test/debug purpose
        static int get_height(Node* n) {
            if (n == nullptr)
                return 0;
            return std::max(get_height(n->links[0]), get_height(n->links[1]))+1;
        }
    };

    typedef Node* node_pointer;
    typedef Node  node_type;
    using Node_alloc_type = Alloc<node_type>;

    Node_alloc_type  node_allocator;
    node_pointer     root = nullptr;
    size_t           node_count = 0;

    void create_node(node_pointer* parent_ptr, const value_type& key);
    void recursive_clear(node_pointer start_node);

    static const key_type& get_key_impl(const value_type& v, std::true_type) {
        return v;
    }
    static const key_type& get_key_impl(const value_type& v, std::false_type) {
        return v.first;
    }
    static const key_type& get_key(const value_type& v) {
        return get_key_impl(v, std::is_void<mapped_type>());
    }

    static int get_direction(node_pointer node, const key_type& key) {
        return key < get_key(node->value)? 0 : 1;
    }
    static node_pointer get_next(node_pointer node, const key_type& key) {
        return node->links[get_direction(node, key)];
    }
    static node_pointer lookup(node_pointer node, const key_type& key) {
        while (node != nullptr && key != get_key(node->value))
            node = get_next(node, key);
        return node;
    }

    //TODO: move to avl_balancer
    static node_pointer avl_rotate_2(node_pointer* path_top, int dir) {
        auto node_B = *path_top;
        auto node_D = node_B->links[dir];
        auto node_C = node_D->links[1-dir];
        auto node_E = node_D->links[dir];

        *path_top = node_D;
        node_D->links[1-dir] = node_B;
        node_B->links[dir]   = node_C;

        node_B->balance = -1;
        node_D->balance = -1;

        return node_E;
    }

    //TODO: move to avl_balancer
    static node_pointer avl_rotate_3(node_pointer* path_top, int dir, int third) {
        auto node_B = *path_top;
        auto node_F = node_B->links[dir];
        auto node_D = node_F->links[1-dir];
        /* note: C and E can be nullptr */
        auto node_C = node_D->links[1-dir];
        auto node_E = node_D->links[dir];
        *path_top = node_D;
        node_D->links[1-dir] = node_B;
        node_D->links[dir]   = node_F;
        node_B->links[dir]   = node_C;
        node_F->links[1-dir] = node_E;
        node_D->balance = -1;
        /* assume both trees are balanced */
        node_B->balance = node_F->balance = -1;
        if(third == -1)
            return nullptr;
        else if (third == dir) {
            /* E holds the insertion so B is unbalanced */
            node_B->balance = 1-dir;
            return node_E;
        } else {
            /* C holds the insertion so F is unbalanced */
            node_F->balance = dir;
            return node_C;
        }
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

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
BinaryTree<Key, T, B, Alloc>::BinaryTree()
{}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
BinaryTree<Key, T, B, Alloc>::~BinaryTree()
{
    clear();
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
void BinaryTree<Key, T, B, Alloc>::create_node(BinaryTree<Key, T, B, Alloc>::node_pointer* parent_ptr, const BinaryTree<Key, T, B, Alloc>::value_type& value) {
    auto new_node = node_allocator.allocate(1, 0);
    node_allocator.construct(new_node, value);
    ++node_count;
    *parent_ptr = new_node;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
bool BinaryTree<Key, T, B, Alloc>::contains(const BinaryTree<Key, T, B, Alloc>::key_type& key) const
{
    return BinaryTree<Key, T, B, Alloc>::lookup(root, key) != nullptr;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
size_t BinaryTree<Key, T, B, Alloc>::size() const
{
    return node_count;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
bool BinaryTree<Key, T, B, Alloc>::insert(const BinaryTree<Key, T, B, Alloc>::value_type& value)
{
    //Stage 1. Find a position in the tree and link a new node
    // by the way find and remember a node where the tree starts to be unbalanced.
    const auto& key = get_key(value);
    auto node_ptr   = &root;
    auto path_top   = &root;
    auto node       = root;
    while (node != nullptr && key != get_key(node->value)){
        if (!node->is_balanced())
            path_top = node_ptr;
        auto dir = get_direction(node, key);
        node_ptr = &(node->links[dir]);
        node = *node_ptr;
    }
    if (node != nullptr)
        return  false; //already has the key
    create_node(node_ptr, value);

    //TODO: move to avl_balancer
    //Stage 2. Rebalance
    //rebalance_insert(path_top, key);
    auto path = *path_top;
    int first, second, third;
    if (path->is_balanced())
        ;
    else if (path->get_balance() != (first = get_direction(path, key))) {
        /* took the shorter path */
        path->balance = -1;
        path = path->links[first];
    } else if (first == (second = get_direction(path->links[first], key))) {
        /* just a two-point rotate */
        path = avl_rotate_2(path_top, first);
    } else {
        /* fine details of the 3 point rotate depend on the third step.
         * However there may not be a third step, if the third point of the
         * rotation is the newly inserted point.  In that case we record
         * the third step as NEITHER
         */
        path = path->links[first]->links[second];
        if (key == get_key(path->value))
            third = -1;
        else
            third = get_direction(path, key);

        path = avl_rotate_3(path_top, first, third);
    }

    //TODO: move to avl_balancer
    //Stage 3. Update balance info in the each node
    while (path != nullptr && key != get_key(path->value)) {
        auto direction = get_direction(path, key);
        path->balance = direction;
        path = path->links[direction];
    }
    return  true;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
void BinaryTree<Key, T, B, Alloc>::clear()
{
    recursive_clear(root);
    root = nullptr;
    node_count = 0;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
void BinaryTree<Key, T, B, Alloc>::recursive_clear(node_pointer start_node) {
    if(start_node == nullptr)
        return;

    if(start_node->links[0] != nullptr)
        recursive_clear(start_node->links[0]);

    if(start_node->links[1] != nullptr)
        recursive_clear(start_node->links[1]);

    node_allocator.destroy(start_node);
    node_allocator.deallocate(start_node, 1);
}
