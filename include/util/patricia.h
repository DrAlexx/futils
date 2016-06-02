#pragma once

#include <functional>
#include <memory>
#include <iterator>
#include <stddef.h>

#include "bitutil.h"

/**
 *
 */
template < class KEY, class Alloc = std::allocator<KEY> >
class patricia_tree
{
public:
    typedef KEY key_type;
    typedef utils::BitStreamAdaptor<KEY> BitStream;

    template <class Tp>
    struct Node
    {
        Node(const Tp& k)
        {
            left     = nullptr;
            right    = nullptr;
            position = 0;
            key      = k;
        }

        Node   *left;
        Node   *right;
        size_t position;
        Tp     key;
    };

    typedef Node<key_type> node_type;

    patricia_tree()
    { root_node = nullptr; }

    ~patricia_tree()
    {
        clear();
    }

    void clear()
    {
        if(root_node == nullptr) return;

        recursive_traverse(root_node, [this](node_type* node)
            {
                node_allocator.destroy(node);
                node_allocator.deallocate(node, 1);
            });
    }

    /**
     * @brief isContain check a tree contains a key argument
     * @param key
     * @return returns true when a tree contains a key otherwise returns false
     */
    bool isContain(const KEY& k)const
    {
       auto node = lookUp(root_node, k);
       return node != nullptr && node->key == k;
    }

    void insert(const KEY& k)
    {
        auto match_node = lookUp(root_node, k);

        //If the key is already present do nothing
        if( match_node != nullptr
                && match_node->key == k ) return;

        //create a new node
        auto new_node = node_allocator.allocate(1, 0);
        node_allocator.construct(new_node, k);

        BitStream key(k);

        //Define bit-position in the new node
        if(match_node != nullptr){
            auto new_pos = key.mismatch(match_node->key);
            new_node->position = new_pos;
            if(new_pos == -1){
                //keys are prefix for each to other
                if(new_node->key.size() < match_node->key.size()) {
                    //OOps! the trie already contains longest key
                    //Need to delete long key insert short key and insert long again
                    //Sample: trie contains AAA and try to insert AA
                    return;
                }
                new_node->position = match_node->key.size();
            }
            if(new_pos == match_node->position){
                new_node->position = match_node->position+1;
            }
        }else{
            //New node hasn't prefix in the trie
            new_node->position = 0;
            if(key.bit(new_node->position)) {
                new_node->right = new_node;
                new_node->left  = root_node;
            } else {
                new_node->left  = new_node;
                new_node->right = root_node;
            }
            root_node = new_node;
            return;
        }

        auto parent_node = lookUp(root_node, k, [&new_node](const node_type* node)
        {
            return new_node->position <= node->position;
        });


        //Connect to the parent
        node_type* next_node;
        if(key.bit(parent_node->position)) {
            next_node = parent_node->right;
            parent_node->right = new_node;
        } else {
            next_node = parent_node->left;
            parent_node->left = new_node;
        }

        //Connect a next node to the new
        if(key.bit(new_node->position)) {
            new_node->right = new_node;
            new_node->left  = next_node;
        } else {
            new_node->left  = new_node;
            new_node->right = next_node;
        }
    }

    void dump(std::ostream& os) {
        os << "digraph G { " << std::endl;
        if(root_node != nullptr)
            recursive_traverse(root_node, [&os](node_type* node)
                {
                if(node->left != nullptr){
                    os << "\"key=" << node->key << ", pos=" << node->position << "\" -> ";
                    os << "\"key=" << node->left->key << ", pos=" << node->left->position << "\";" << std::endl;
                }
                if(node->right != nullptr){
                    os << "\"key=" << node->key << ", pos=" << node->position << "\" -> ";
                    os << "\"key=" << node->right->key << ", pos=" << node->right->position << "\";" << std::endl;
                }
                if(node->right == nullptr && node->left == nullptr){
                    os << "\"key=" << node->key << ", pos=" << node->position << "\";" << std::endl;
                }
                });

        os << "}" << std::endl;
    }

private:
    typedef typename Alloc::template rebind< Node<KEY> >::other Node_alloc_type;
    Node_alloc_type node_allocator;
    node_type       *root_node;

    /**
     * @brief lookUp
     * @param start_node
     * @param k
     * @param visitor is a functor that receives const node_type* argument.
     *  when visitor returns true lookUp interupts node searchig and returns a current node.
     * @return
     */
    node_type* lookUp(node_type* start_node, const KEY& k,
                      std::function<bool(const node_type*)> visitor=[](const node_type*)
                      { return false; })
    {
        if(start_node == nullptr) return start_node;

        auto node = start_node;
        BitStream key(k);

        while(node->position < key.size()
              && !visitor(node))
        {
            auto next_node = key.bit(node->position)? node->right : node->left;
            if(next_node == nullptr) break;
            if(next_node->position <= node->position){
                //Leaf (link up) found
                node = next_node;
                break;
            }
            node = next_node;
        }

        return node;
    }

    /**
     * @brief recursive_traverse
     * @param start_node assume never is nullptr
     */
    void recursive_traverse(node_type* start_node, std::function<void(node_type*)> visitor)
    {
        if(start_node->left != nullptr
                && start_node->left->position > start_node->position)
            recursive_traverse(start_node->left, visitor);

        if(start_node->right != nullptr
                && start_node->right->position > start_node->position)
            recursive_traverse(start_node->right, visitor);

        visitor(start_node);
    }

};
