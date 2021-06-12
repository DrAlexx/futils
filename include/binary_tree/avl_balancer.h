#pragma once

#include "base.h"

namespace binary_tree {

struct avl_balancer {

struct Node {
    auto get_balance() const { return balance; }
    bool is_balanced() const { return balance < 0; }
    void set_balance(int8_t new_balance) {
        balance = new_balance;
    }

private:
    /*
     * -1 means balanced
     * 1  means right is highier
     * 0  means left is highier
     */
    int8_t balance = -1;
};

private:
template <typename Node>
static Node* avl_rotate_2(Node** path_top, int dir) noexcept {
    (*path_top)->set_balance(-1);
    Node* result = rotate_2(path_top, dir);
    (*path_top)->set_balance(-1);
    return result;
}

template <typename Node>
static Node* avl_rotate_3(Node** path_top, int dir, int third) noexcept {
    auto node_B = *path_top;
    auto node_F = node_B->links[dir];
    auto node_D = node_F->links[1 - dir];
    /* note: C and E can be nullptr */
    auto node_C = node_D->links[1 - dir];
    auto node_E = node_D->links[dir];

    node_B->set_balance(-1);
    node_F->set_balance(-1);
    node_D->set_balance(-1);

    rotate_3(path_top, dir);

    if(third == -1)
        return nullptr;
    else if (third == dir) {
        /* E holds the insertion so B is unbalanced */
        node_B->set_balance(1 - dir);
        return node_E;
    } else {
        /* C holds the insertion so F is unbalanced */
        node_F->set_balance(dir);
        return node_C;
    }
}

public:
template<typename Node, typename C>
static bool insert(Node** root, typename Node::value_type value, C create_node)
{
    //Stage 1. Find a position in the tree and link a new node
    // by the way find and remember a node where the tree starts to be unbalanced.
    const auto& key = Node::get_key(value);
    auto node_ptr   = root;
    auto path_top   = root;
    auto node       = *root;
    while (node != nullptr && key != Node::get_key(node->value)){
        if (!node->is_balanced())
            path_top = node_ptr;
        auto dir = node->get_direction(key);
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
    else if (path->get_balance() != (first = path->get_direction(key))) {
        /* took the shorter path */
        path->set_balance(-1);
        path = path->links[first];
    } else if (first == (second = path->links[first]->get_direction(key))) {
        /* just a two-point rotate */
        path = avl_rotate_2(path_top, first);
    } else {
        /* fine details of the 3 point rotate depend on the third step.
         * However there may not be a third step, if the third point of the
         * rotation is the newly inserted point.  In that case we record
         * the third step as NEITHER
         */
        path = path->links[first]->links[second];
        if (key == Node::get_key(path->value))
            third = -1;
        else
            third = path->get_direction(key);

        path = avl_rotate_3(path_top, first, third);
    }

    //Stage 3. Update balance info in the each node
    while (path != nullptr && key != Node::get_key(path->value)) {
        auto direction = path->get_direction(key);
        path->set_balance(direction);
        path = path->links[direction];
    }
    return  true;
}

template<typename Node>
static Node* erase(Node** root, const typename Node::key_type& key) noexcept
{
    //Stage 1. lookup for the node that contain a key
    auto node                  = *root;
    auto nodep                 = root;
    auto path_top              = root;
    decltype(path_top) targetp = nullptr;
    int dir = 0;

    while (node) {
        dir = node->get_direction(key);
        if (key == Node::get_key(node->value))
            targetp = nodep;
        if (node->links[dir] == nullptr)
            break;
        if (node->is_balanced()
            || (node->get_balance() == (1-dir) && node->links[1-dir]->is_balanced())
            ) path_top = nodep;
        nodep = &node->links[dir];
        node = *nodep;
    }
    if (targetp == nullptr)
        return 0; //key not found nothing to remove

    /*
     * Stage 2.
     * adjust balance, but don't lose 'targetp'.
     * each node from treep down towards target, but
     * excluding the last, will have a subtree grow
     * and need rebalancing
     */
    auto treep = path_top;
    auto targetn = *targetp;
    while(true) {
        auto tree = *treep;
        auto bdir = tree->get_direction(key);
        if (tree->links[bdir] == nullptr)
            break;
        if (tree->is_balanced()) {
            tree->set_balance(1 - bdir);
        } else if (tree->get_balance() == bdir) {
            tree->set_balance(-1);
        } else {
            auto second = tree->links[1 - bdir]->get_balance();
            if (second == bdir) {
                avl_rotate_3(treep, 1 - bdir, tree->links[1 - bdir]->links[bdir]->get_balance());
            } else if(second == -1) {
                avl_rotate_2(treep, 1 - bdir);
                tree->set_balance(1 - bdir);
                (*treep)->set_balance(bdir);
            } else {
                avl_rotate_2(treep, 1 - bdir);
            }
            if (tree == targetn) {
                targetp = &(*treep)->links[bdir];
            }
        }
        treep = &(tree->links[bdir]);
    }

    /*
     * Stage 3.
     * We have re-balanced everything, it remains only to
     * swap the end of the path (*treep) with the deleted item
     * (*targetp)
     */
    auto tree = *treep;
    targetn   = *targetp;
    *targetp  = tree;
    *treep    = tree->links[1 - dir];
    tree->links[0] = targetn->links[0];
    tree->links[1] = targetn->links[1];
    tree->set_balance(targetn->get_balance());

    return targetn;
}
};

}
