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
        recursive_clear(root_node);
    }

    /**
     * @brief isContain check atree contains a key argument
     * @param key
     * @return returns true when a tree contains a key otherwise returns false
     */
    bool isContain(const KEY& k)const
    {
       auto node = lookUp(root_node, k);
       return node != nullptr && node->key == k;f
    }

    void insert(const KEY& k)
    {
        auto match_node = lookUp(root_node, k);

        //If the key is already present do nothing
        if( match_node != nullptr
                 && match_node->key == k) return;

        //create a new node
        auto new_node = node_allocator.allocate(1, 0);
        node_allocator.construct(new_node, k);

        utils::BitStreamAdaptor key(k);

        //Define bit-position in the new node
        if(match_node != nullptr){
            new_node->position = key.mismatch(match_node->key);
            if(new_node->position == -1){
                //keys are prefix for each to other
                if(new_node->key.size() < match_node->key.size()) {
                    //OOps! the trie already contains longest key
                    //Need to delete long key insert short key and insert long again
                    //Sample: trie contains AAA and try to insert AA
                    return;
                }
                new_node->position = match_node->key.size();
            }
        }else{
            //New node hasn't prefix in the trie
            new_node->position = 0;
        }

        auto parent_node = lookUp(root_node, key, [&new_node](const node_type* node)
        {
            return new_node->position > node->position;
        });
        

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
    const node_type* lookUp(const node_type* start_node, const KEY& k,
                            std::function<bool(const node_type*)> visitor=[](const node_type*)
                            { return false; }) const
    {
        if(start_node == nullptr) return start_node;

        auto node = start_node;
        utils::BitStreamAdaptor key(k);

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
     * @brief recursive_clear
     * @param start_node assume never is nullptr
     */
    void recursive_clear(node_type* start_node)
    {
        if(start_node->left != nullptr
                && start_node->left->position > start_node->position)
            recursive_clear(start_node->left);

        if(start_node->right != nullptr
                && start_node->right->position > start_node->position)
            recursive_clear(start_node->right);

        node_allocator.destroy(start_node);
        node_allocator.deallocate(start_node);
    }

};
