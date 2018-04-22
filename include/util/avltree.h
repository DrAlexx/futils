#pragma once

#include <utility>
#include <memory>
#include <functional>
#include <cstdlib>
#include <stdint.h>

template<typename T, typename Alloc = std::allocator<T>>
class AVLTree
{
public:
    typedef T value_type;

    AVLTree();
    ~AVLTree();

    void insert(const value_type& v);
    void erase(const value_type& key);
    const value_type& find(const T& key);
    void clear();

private:
    struct Node {
        enum eLinkType {
            LINK_LEFT  = 0,
            LINK_RIGHT = 1,
        };
        Node*       links[2] = {nullptr, nullptr};
        value_type  key;
        int         height   = 1;

        Node(const T& v)
            : key(v)
        {};
    };
    enum eRotation {
        ROTATION_LEFT  = 0,
        ROTATION_RIGHT = 1,
    };
    typedef Node* node_pointer;
    typedef Node  node_type;
    typedef typename Alloc::template rebind< node_type >::other Node_alloc_type;
    node_pointer     root = nullptr;
    Node_alloc_type node_allocator;

    void recursive_traverse(node_pointer start_node, std::function<void(node_pointer)> visitor);
    node_pointer recursive_insert(node_pointer node, const value_type& key);

    static int get_node_height(const node_pointer node);
    static int get_node_balance(const node_pointer node);
    static node_pointer rotate(node_pointer node, eRotation r);
    static node_pointer do_balance(node_pointer node, const value_type& key);
};

template<typename T, typename Alloc>
AVLTree<T, Alloc>::AVLTree()
{}

template<typename T, typename Alloc>
AVLTree<T, Alloc>::~AVLTree()
{
    clear();
}

template<typename T, typename Alloc>
int AVLTree<T, Alloc>::get_node_height(const AVLTree<T, Alloc>::node_pointer node)
{
    if(node == nullptr)
        return 0;
    return node->height;
}

template<typename T, typename Alloc>
int AVLTree<T, Alloc>::get_node_balance(const AVLTree<T, Alloc>::node_pointer node)
{
    if(node == nullptr)
        return 0;
    return get_node_height(node->links[0]) - get_node_height(node->links[1]);
}

template<typename T, typename Alloc>
typename AVLTree<T, Alloc>::node_pointer AVLTree<T, Alloc>::rotate(AVLTree<T, Alloc>::node_pointer node, AVLTree<T, Alloc>::eRotation r)
{
    auto new_root = node->links[1-r];
    auto other    = new_root->links[r];

    // Perform rotation
    new_root->links[r] = node;
    node->links[1-r]   = other;

    // Update heights
    node->height     = std::max(get_node_height(node->links[0]),
            get_node_height(node->links[1]))+1;
    new_root->height = std::max(get_node_height(new_root->links[0]),
            get_node_height(new_root->links[1]))+1;

    // Return new root
    return new_root;
}

template<typename T, typename Alloc>
typename AVLTree<T, Alloc>::node_pointer AVLTree<T, Alloc>::do_balance(AVLTree<T, Alloc>::node_pointer node, const AVLTree<T, Alloc>::value_type& key)
{
    auto factor = get_node_balance(node);

    if(factor > 1) {
        if(key < node->links[0]->key)
            return rotate(node, AVLTree<T, Alloc>::ROTATION_RIGHT);
        if(key > node->links[0]->key) {
            node->links[0] = rotate(node->links[0], AVLTree<T, Alloc>::ROTATION_LEFT);
            return rotate(node, AVLTree<T, Alloc>::ROTATION_RIGHT);
        }
    }

    if(factor < -1) {
        if(key > node->links[1]->key)
            return rotate(node, AVLTree<T, Alloc>::ROTATION_LEFT);
        if(key < node->links[1]->key) {
            node->links[1] = rotate(node->links[1], AVLTree<T, Alloc>::ROTATION_RIGHT);
            return rotate(node, AVLTree<T, Alloc>::ROTATION_LEFT);
        }
    }
    return node;
}

template<typename T, typename Alloc>
void AVLTree<T, Alloc>::insert(const AVLTree<T, Alloc>::value_type& key)
{
    root = recursive_insert(root, key);
}

template<typename T, typename Alloc>
void AVLTree<T, Alloc>::erase(const AVLTree::value_type& key)
{
    if(root == nullptr)
        return;
}

//template<typename T, typename Alloc>
//const value_type& AVLTree<T,Alloc>::find(const K& key)
//{

//}

template<typename T, typename Alloc>
void AVLTree<T, Alloc>::clear()
{
    recursive_traverse(root, [this](node_pointer node){
        node_allocator.destroy(node);
        node_allocator.deallocate(node, 1);
    });
}

template<typename T, typename Alloc>
void AVLTree<T, Alloc>::recursive_traverse(node_pointer start_node, std::function<void(node_pointer)> visitor) {
    if(start_node == nullptr)
        return;

    if(start_node->links[0] != nullptr)
        recursive_traverse(start_node->links[0], visitor);

    if(start_node->links[1] != nullptr)
        recursive_traverse(start_node->links[1], visitor);

    visitor(start_node);
}

template<typename T, typename Alloc>
typename AVLTree<T, Alloc>::node_pointer AVLTree<T, Alloc>::recursive_insert(typename AVLTree<T, Alloc>::node_pointer node, const value_type& key)
{
    if(node == nullptr) {
        //create a new node
        auto new_node = node_allocator.allocate(1, 0);
        node_allocator.construct(new_node, key);
        return new_node;
    }

    if(key < node->key) {
        node->links[0] = recursive_insert(node->links[0], key);
    } else if(key > node->key) {
        node->links[1] = recursive_insert(node->links[1], key);
    } else {
        //already contains
        return node;
    }

    node->height = 1 + std::max(get_node_height(node->links[0]),
            get_node_height(node->links[1]));

    return do_balance(node, key);
}
