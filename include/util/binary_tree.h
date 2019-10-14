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
    void insert(const value_type& key);
    void erase(const key_type& key);
    void clear();

    //Const
    bool contains(const key_type& key) const;
    size_t size() const;

    //Iteratots
    //TODO

    void dump_tree (std::ostringstream& ss) const {
        enum eDirection {
            DIR_DOWN,
            DIR_UP,
        };
        eDirection dir = DIR_DOWN;
        ss << "digraph BST {\n";
        auto node = root;
        ss << "root -> {" << '"' << get_key(node->value) << '/' << (int)node->height << "\"}\n";
        while (node != nullptr)
        {
            if (dir == DIR_DOWN) {
                ss << '"' << get_key(node->value) << '/' << (int)node->height << "\" -> { " ;
                if (node->up != nullptr) {
                    ss << '"' << get_key(node->up->value) << '/' << (int)node->up->height << "\" ";
                }
                if (node->links[0] != nullptr) {
                    ss << '"' << get_key(node->links[0]->value) << '/' << (int)node->links[0]->height << "\" ";
                }
                if (node->links[1] != nullptr) {
                    ss << '"' << get_key(node->links[1]->value) << '/' << (int)node->links[1]->height << "\" ";
                }
                ss << "}\n";
            } else {
                if(node->up == nullptr)
                    break;
                if(node->up->links[0] == node) {
                    dir = DIR_DOWN;
                    node = node->up->links[1];
                } else {
                    node = node->up;
                    dir = DIR_UP;
                }
                continue;
            }

            if(node->links[0] != nullptr) {
                node = node->links[0];
            } else if(node->links[1] != nullptr) {
                node = node->links[1];
            } else {
                dir = DIR_UP;
            }
        }

        ss << "}\n";
    }
private:
    struct Node : public balancer_type::NodeInfo {
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

    Node_alloc_type  node_allocator;
    node_pointer     root = nullptr;
    size_t           node_count = 0;

    static node_pointer rotate(node_pointer node, eRotation r);
    static std::tuple<node_pointer, node_pointer> find_node(node_pointer start_node, const key_type& key);
    static node_pointer balance(node_pointer node, const key_type& key);

    //TODO: non-recursive insert/erase
//    node_pointer recursive_insert(node_pointer node, const value_type& key);
    node_pointer recursive_erase(node_pointer node, const key_type& key);

    static const key_type& get_key_impl(const value_type& v, std::true_type) {
        return v;
    }
    static const key_type& get_key_impl(const value_type& v, std::false_type) {
        return v.first;
    }
    static const key_type& get_key(const value_type& v) {
        return get_key_impl(v, std::is_void<mapped_type>());
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

template<typename Key, typename T,  typename B, template<typename X> typename Alloc>
typename BinaryTree<Key, T, B, Alloc>::node_pointer
BinaryTree<Key, T, B, Alloc>::rotate(BinaryTree<Key, T, B, Alloc>::node_pointer node, BinaryTree<Key, T, B, Alloc>::eRotation r)
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
    balancer_type::update_node_info(node);
    balancer_type::update_node_info(new_root);

    return new_root;
}

template<typename Key, typename T,  typename B, template<typename X> typename Alloc>
std::tuple<typename BinaryTree<Key, T, B, Alloc>::node_pointer, typename BinaryTree<Key, T, B, Alloc>::node_pointer>
BinaryTree<Key, T, B, Alloc>::find_node(BinaryTree<Key, T, B, Alloc>::node_pointer root, const BinaryTree<Key, T, B, Alloc>::key_type& key) {
    if (root == nullptr)
        return std::make_tuple(nullptr, nullptr);

    BinaryTree<Key, T, B, Alloc>::node_pointer parent{root};
    BinaryTree<Key, T, B, Alloc>::node_pointer node{nullptr};

    if(get_key(parent->value) == key)
       return std::make_tuple(nullptr, parent);

    do {
        node = key < get_key(parent->value) ? parent->links[0] : parent->links[1];
        if (node == nullptr)
            break;
        if (key == get_key(node->value))
            return std::make_tuple(parent, node);
        parent = node;
    } while (true);

    return std::make_tuple(parent, node);
}

template<typename Key, typename T,  typename B, template<typename X> typename Alloc>
typename BinaryTree<Key, T, B, Alloc>::node_pointer
BinaryTree<Key, T, B, Alloc>::balance(typename BinaryTree<Key, T, B, Alloc>::node_pointer node, const typename BinaryTree<Key, T, B, Alloc>::key_type& key)
{
    auto factor = BinaryTree<Key, T, B, Alloc>::balancer_type::get_balance_factor(node);

    if(factor > 1) {
        if(key < get_key(node->links[0]->value)) {
            node = rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
        }
        if(key > get_key(node->links[0]->value)) {
            node->links[0] = rotate(node->links[0], BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
            node = rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
        }
    }

    if(factor < -1) {
        if(key > get_key(node->links[1]->value)) {
            node = rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
        }
        if(key < get_key(node->links[1]->value)) {
            node->links[1] = rotate(node->links[1], BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
            node = rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
        }
    }

    return node;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
void BinaryTree<Key, T, B, Alloc>::insert(const BinaryTree<Key, T, B, Alloc>::value_type& value)
{
    const auto& key = get_key(value);

    //STEP 1: search node
    auto [parent, node] = find_node(root, key);
    if(node != nullptr)
        return; //Done when already contains

    //STEP 2: Create a new node
    auto new_node = node_allocator.allocate(1, 0);
    node_allocator.construct(new_node, value);
    ++node_count;
    new_node->up = parent;

    //STEP 3: Link a new node to the tree
    if(parent != nullptr) {
        if(key < get_key(parent->value))
            parent->links[0] = new_node;
        else
            parent->links[1] = new_node;
    } else {
        root = new_node;
        return;
    }

    //STEP 4: Get up and rebalance the tree
    node = parent;
    do {
        BinaryTree<Key, T, B, Alloc>::balancer_type::update_node_info(node);
        if(key < get_key(node->value)) {
            node->links[0] = balance(node->links[0], key);
            node->links[0]->up = node;
        } else {
            node->links[1] = balance(node->links[1], key);
            node->links[1]->up = node;
        }
        BinaryTree<Key, T, B, Alloc>::balancer_type::update_node_info(node);
        if (node->up == nullptr) {
            BinaryTree<Key, T, B, Alloc>::balancer_type::update_node_info(node);
            root = balance(node, key);
            root->up = nullptr;
            BinaryTree<Key, T, B, Alloc>::balancer_type::update_node_info(root);
            return;
        }
        node = node->up;
    } while (true);
    BinaryTree<Key, T, B, Alloc>::balancer_type::update_node_info(node);
    root = node;
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

    if(key < get_key(node->value)) {
        node->links[0] = recursive_erase(node->links[0], key);
    } else if(key > get_key(node->value)) {
        node->links[1] = recursive_erase(node->links[1], key);
    } else {
          // node with only one child or no child
          if( (node->links[0] == nullptr) || (node->links[1] == nullptr) ) {
              auto temp = node->links[0] ? node->links[0] : node->links[1];

              // No child case
              if (temp == nullptr) {
                  temp = node;
                  node = nullptr;
              } else {
                  // One child case. Copy temp node into node
                  node->value = std::move(temp->value);
                  node->links[0] = nullptr;
                  node->links[1] = nullptr;
                  balancer_type::update_node_info(node);
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
              node->value = std::move(temp->value);

              // Delete the inorder successor
              root->links[1] = recursive_erase(node->links[1], get_key(temp->value));
          }
    }

    // If the tree had only one node then return
       if (node == nullptr)
         return node;

       balancer_type::update_node_info(root);
       auto factor = balancer_type::get_balance_factor(node);

       if (factor > 1) {
           if (balancer_type::get_balance_factor(node->links[0]) >= 0) {
               // Left Left Case
               return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
           } else {
               // Left Right Case
               node->links[0] =  rotate(node->links[0], BinaryTree<Key, T, B, Alloc>::ROTATION_LEFT);
               return rotate(node, BinaryTree<Key, T, B, Alloc>::ROTATION_RIGHT);
           }
       }

       if (factor < -1) {
           if (balancer_type::get_balance_factor(node->links[0]) <= 0) {
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
    auto node = root;
    while (node != nullptr) {
        if (node->links[0] != nullptr) {
            node = node->links[0];
            continue;
        }
        if (node->links[1] != nullptr) {
            node = node->links[1];
            continue;
        }
        auto up = node->up;
        if(up != nullptr) {
            if (up->links[0] == node) {
                up->links[0] = nullptr;
            } else if (up->links[1] == node) {
                up->links[1] = nullptr;
            }
        }

        node_allocator.destroy(node);
        node_allocator.deallocate(node, 1);
        node = up;
    }
    root = nullptr;
    node_count = 0;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
bool BinaryTree<Key, T, B, Alloc>::contains(const key_type& key) const
{
    auto node = root;
    while (node != nullptr)
    {
        if( get_key(node->value) == key) {
            return true;
        }
        node = key < get_key(node->value) ? node->links[0] : node->links[1];
    }

    return false;
}

template<typename Key, typename T, typename B, template<typename X> typename Alloc>
size_t BinaryTree<Key, T, B, Alloc>::size() const
{
    return node_count;
}
