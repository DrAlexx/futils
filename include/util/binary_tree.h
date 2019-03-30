#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "avl_tree_balancer.h"

template<typename Key, typename T, typename B = AVLTreeBalancer, template<typename X> typename Alloc = std::allocator>
class BinaryTree
{
public:
    typedef Key key_type;
    typedef T   mapped_type;

    template <typename K>
    struct SetValueType {
        SetValueType(const K& k)
            :key(k){}
        K key;
    };
    template <typename K, typename V>
    struct MapValueType {
        K key;
        V value;
    };
    using value_type = typename std::conditional<
        std::is_void<mapped_type>::value,
        SetValueType<key_type>,
        MapValueType<key_type, mapped_type>>::type;

    //Construct
    BinaryTree();
    ~BinaryTree();

    //Modifiers
    void insert(const value_type& key);
    void erase(const key_type& key);
    void clear();

    //Const
    bool contains(const key_type& key) const;
    size_t size() const;

    //Iteratots
    //TODO

private:
    struct Node : public B::NodeInfo {
        Node*       links[2] = {nullptr, nullptr};
        Node*       up       = nullptr;
        value_type  value;

        Node(const value_type& v)
            : value(v)
        {}
    };
    enum eRotation {
        ROTATION_LEFT  = 0,
        ROTATION_RIGHT = 1,
    };

    typedef Node* node_pointer;
    typedef Node  node_type;
    using Node_alloc_type = Alloc<node_type>;
    node_pointer     root = nullptr;
    size_t           node_count = 0;
    Node_alloc_type  node_allocator;

    static node_pointer rotate(node_pointer node, eRotation r);

    void recursive_clear(node_pointer start_node);
    node_pointer recursive_insert(node_pointer node, const value_type& key);
    node_pointer recursive_erase(node_pointer node, const key_type& key);
};

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
BinaryTree<Key, T, B, Alloc>::BinaryTree()
{}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
BinaryTree<Key, T, B, Alloc>::~BinaryTree()
{
    clear();
}

template<typename Key, typename T,  typename B, template<typename X> typename Alloc>
typename BinaryTree<Key, T, B, Alloc>::node_pointer BinaryTree<Key, T, B, Alloc>::rotate(BinaryTree<Key, T, B, Alloc>::node_pointer node,
                                                                             BinaryTree<Key, T, B, Alloc>::eRotation r)
{
    auto new_root = node->links[1-r];
    auto other    = new_root->links[r];

    // Perform rotation
    new_root->links[r] = node;
    if(node != nullptr)
        node->up = new_root;
    node->links[1-r]   = other;
    if (other != nullptr)
        other->up = node;

    // Update info
    B::update_node_info(node);
    B::update_node_info(new_root);

    return new_root;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
void BinaryTree<Key, T, B, Alloc>::insert(const BinaryTree<Key, T, B, Alloc>::value_type& value)
{
    root = recursive_insert(root, value);
    root->up = nullptr;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
typename BinaryTree<Key, T, B, Alloc>::node_pointer BinaryTree<Key, T, B, Alloc>::recursive_insert(typename BinaryTree<Key, T, B, Alloc>::node_pointer node, const value_type& value)
{
    if(node == nullptr) {
        //create a new node
        auto new_node = node_allocator.allocate(1, 0);
        node_allocator.construct(new_node, value);
        ++node_count;
        return new_node;
    }

    if(value.key < node->value.key) {
        node->links[0] = recursive_insert(node->links[0], value);
        node->links[0]->up = node;
    } else if(value.key > node->value.key) {
        node->links[1] = recursive_insert(node->links[1], value);
        node->links[1]->up = node;
    } else {
        //already contains
        return node;
    }

    B::update_node_info(node);

    auto factor = B::get_balance_factor(node);

    if(factor > 1) {
        if(value.key < node->links[0]->value.key)
            return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
        if(value.key > node->links[0]->value.key) {
            node->links[0] = rotate(node->links[0], BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
            return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
        }
    }

    if(factor < -1) {
        if(value.key > node->links[1]->value.key)
            return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
        if(value.key < node->links[1]->value.key) {
            node->links[1] = rotate(node->links[1], BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
            return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
        }
    }
    return node;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
void BinaryTree<Key, T, B, Alloc>::erase(const BinaryTree::key_type& key)
{
    root = recursive_erase(root, key);
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
typename BinaryTree<Key, T, B, Alloc>::node_pointer BinaryTree<Key, T, B, Alloc>::recursive_erase(typename BinaryTree<Key, T, B, Alloc>::node_pointer node, const key_type& key)
{
    if(node == nullptr)
        return nullptr;

    if(key < node->value.key) {
        node->links[0] = recursive_erase(node->links[0], key);
    } else if(key > node->value.key) {
        node->links[1] = recursive_erase(node->links[1], key);
    } else {
          // node with only one child or no child
          if( (node->links[0] == nullptr) || (node->links[1] == nullptr) ) {
              auto temp = node->links[0] ? node->links[0] : node->links[1];

              // No child case
              if (temp == nullptr) {
                  temp = node;
                  node = nullptr;
              } else { // One child case
                  //TODO: change links, no body copying?
                  *node = *temp; // Copy the contents of
              }
              // the non-empty child
              node_allocator.destroy(temp);
              node_allocator.deallocate(temp, 1);
              --node_count;
          } else {
              // node with two children: Get the inorder
              // successor (smallest in the right subtree)
              auto temp = node->links[1];

              /* loop down to find the leftmost leaf */
              while (temp->links[0] != nullptr)
                  temp = temp->links[0];

              // Copy the inorder successor's data to this node
              node->value.key = temp->value.key; //TODO: avoid copy

              // Delete the inorder successor
              root->links[1] = recursive_erase(node->links[1], temp->value.key);
          }
    }

    // If the tree had only one node then return
       if (node == nullptr)
         return node;

       // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
       B::update_node_info(root);

       // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
       auto factor = B::get_balance_factor(node);

       // If this node becomes unbalanced, then there are 4 cases
       if (factor > 1) {
           if (B::get_balance_factor(node->links[0]) >= 0) {
               // Left Left Case
               return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
           } else {
               // Left Right Case
               node->links[0] =  rotate(node->links[0], BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
               return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
           }
       }

       if (factor < -1) {
           if (B::get_balance_factor(node->links[0]) <= 0) {
               // Right Right Case
               return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
           } else {
               // Right Left Case
               node->links[1] =  rotate(node->links[1], BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
               return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
           }
       }

       return node;
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

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
bool BinaryTree<Key, T, B, Alloc>::contains(const key_type& key) const
{
    auto node = root;
    while (node != nullptr)
    {
        if( node->key == key) {
            return true;
        }
        node = key < node->key ? node->links[0] : node->links[1];
    }

    return false;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
size_t BinaryTree<Key, T, B, Alloc>::size() const
{
    return node_count;
}
