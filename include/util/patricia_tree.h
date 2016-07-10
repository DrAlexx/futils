#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "bitutil.h"

/**
   TODO: 1)Find all strings with common prefix: Returns an array of strings which begin with the same prefix.
   TODO: 2)Find predecessor: Locates the largest string less than a given string, by lexicographic order.
   TODO: 3)Find successor: Locates the smallest string greater than a given string, by lexicographic order.
 */
template<typename KEY, typename Alloc = std::allocator<KEY>>
class patricia_tree
{
public:
    typedef KEY key_type;


private:
    //Tree node definitions
    struct Node
    {
        Node(const KEY& k)
        {
            left     = nullptr;
            right    = nullptr;
            position = 0;
            key      = k;
        }

        Node   *left;
        Node   *right;
        size_t position;
        KEY    key;
    };

    typedef Node                                    node_type;
    typedef node_type*                              node_pointer;
    typedef std::pair<node_pointer, node_pointer>   node_pointer_pair;

public:
    /**
     * @brief patricia_tree creates an empty tree
     */
    patricia_tree();

    /**
     * @brief ~patricia_tree clears the contents
     */
    ~patricia_tree();

    /**
     * @brief clear clears the contents
     */
    void clear();

    /**
     * @brief contains check a tree contains a key
     * @param key for checking
     * @return returns true when a tree contains a key otherwise returns false
     */
    bool contains(const KEY& k) const;

    /**
     * @brief insert isert a new key into the tree.
     * Does nothig when a key is already present.
     * @param k is a key that should be inserted
     */
    void insert(const KEY& k);

    /**
     * @brief erase removes a key from the tree
     * @param k is a key that should be removed
     */
    void erase(const KEY& k);

    /**
     * @brief dump
     * @param os
     */
    void dump(std::ostream& os);

    //Prefix
//    class prefix {
//    public:
//        prefix()=delete;
//        const KEY& key()const
//        {
//            return prefix_key;
//        }
//        void add_key(const KEY& k);
//        std::vector<KEY> keys(size_t = 0) const;

//    private:
//        patricia_tree   *tree;
//        KEY             prefix_key;
//        node_pointer    node;
//        friend class patricia_tree;

//        void

//        prefix(patricia_tree *t, KEY k)
//            :tree(t)
//            ,prefix_key(k)
//            , node(nullptr){}
//    };

private:
    typedef typename Alloc::template rebind< node_type >::other Node_alloc_type;
    typedef utils::BitStreamAdaptor<KEY> BitStream;

    Node_alloc_type node_allocator;
    node_pointer    root_node;

    /**
     * @brief look_up
     * @param start_node
     * @param k
     * @param visitor is a functor that receives const node_pointer argument.
     *  when visitor returns true lookUp interupts node searchig and returns a current node.
     * @return
     */
    node_pointer_pair look_up(node_pointer start_node, const KEY& k,
                              std::function<bool(const node_pointer, const node_pointer)> visitor);

    /**
     * @brief recursive_traverse calls visitor functor for every node in the subtree.
     * The order of traverse is down-to-up and left-to-right.
     * @note Caller must grant start_node isn't nullptr
     * @param start_node is a root of subtree
     * @param visitor is functor that receives a node pointer.
     */
    void recursive_traverse(node_pointer start_node, std::function<void(node_pointer)> visitor);

    /**
     * @brief remove_leaf the tree contains outer and inner nodes.
     * All outer node are leafs. This method removes a node that is leaf.
     * @note Caller must grant node is really leaf and parent is really paren.
     * Otherwise the tree will be damaged.
     * @param parent is a parent node.
     * @param node node pointer that should be remove
     */
    void remove_leaf(node_pointer parent, node_pointer node);
};

template<typename KEY, typename Alloc>
patricia_tree<KEY, Alloc>::patricia_tree() {
    root_node = nullptr;
}

template<typename KEY, typename Alloc>
patricia_tree<KEY, Alloc>::~patricia_tree() {
    clear();
}

template<typename KEY, typename Alloc>
void patricia_tree<KEY, Alloc>::clear() {
    if(root_node == nullptr) return;

    recursive_traverse(root_node, [this](node_pointer node)
    {
        node_allocator.destroy(node);
        node_allocator.deallocate(node, 1);
    });
}

template<typename KEY, typename Alloc>
bool patricia_tree<KEY, Alloc>::contains(const KEY& k) const {
    auto pair = look_up(root_node, k, [](node_pointer node, node_pointer next){
        return next == nullptr || next->position <= node->position;
    });
   return pair.second != nullptr && pair.second->key == k;
}

template<typename KEY, typename Alloc>
void patricia_tree<KEY, Alloc>::insert(const KEY& k) {
    auto pair = look_up(root_node, k, [](node_pointer node, node_pointer next){
            return next == nullptr || next->position <= node->position;
        });
    auto match_node = pair.second == nullptr ? pair.first : pair.second;

    //If the key is already present do nothing
    if( match_node != nullptr && match_node->key == k ) return;

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
                //FIXME: OOps! the trie already contains longest key
                //Need to delete long key insert short key and insert long again
                //Sample: trie contains AAA and try to insert AA
                return;
            }
            new_node->position = match_node->key.size();
        }
        if(static_cast<unsigned>(new_pos) == match_node->position){
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

    auto parent_node = look_up(root_node, k, [&new_node](const node_pointer node, const node_pointer next)
    {
        return next == nullptr || next->position <= node->position
                || (new_node->position >= node->position && new_node->position < next->position);
    }).first;


    //Connect to the parent
    node_pointer next_node;
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

template<typename KEY, typename Alloc>
void patricia_tree<KEY, Alloc>::erase(const KEY& k) {
    auto pair = look_up(root_node, k, [](node_pointer node, node_pointer next){
            return next == nullptr || next->position <= node->position;
        });
    auto parent_node = pair.first;
    auto match_node = pair.second;

    //If the key isn't present do nothing
    if(match_node == nullptr || match_node->key != k ) return;

    BitStream key(k);
    auto link = key.bit(match_node->position)? match_node->right : match_node->left;
    if(link != match_node){
        //match_node isn't external leaf. So we need to swap the node with the external leaf parent
        auto grand_node = look_up(root_node, k, [match_node](node_pointer node, node_pointer next){
                return next == nullptr || next->position <= node->position
                       || next == match_node;
            }).first;

        if(key.bit(grand_node->position)) {
            grand_node->right = parent_node;
        } else {
            grand_node->left = parent_node;
        }

        std::swap(parent_node->position, match_node->position);

        auto next = parent_node->right == match_node? parent_node->left : parent_node->right;
        if(key.bit(parent_node->position)) {
            parent_node->right = match_node;
            parent_node->left  = match_node->left;
        } else {
            parent_node->left  = match_node;
            parent_node->right = match_node->right;
        }

        if(key.bit(match_node->position)) {
            match_node->right = match_node;
            match_node->left  = next;
        } else {
            match_node->right = next;
            match_node->left  = match_node;
        }
    }

    remove_leaf(parent_node, match_node);
}

template<typename KEY, typename Alloc>
void patricia_tree<KEY, Alloc>::dump(std::ostream& os) {
    os << "digraph G { " << std::endl;
    if(root_node != nullptr)
        recursive_traverse(root_node, [&os](node_pointer node)
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

//Private methods
template<typename KEY, typename Alloc>
typename patricia_tree<KEY, Alloc>::node_pointer_pair
patricia_tree<KEY, Alloc>::look_up(node_pointer start_node,
                                  const KEY& k,
                                  std::function<bool(const node_pointer, const node_pointer)> visitor) {
    if(start_node == nullptr) return node_pointer_pair(nullptr, nullptr);

    BitStream key(k);
    node_pointer node = start_node;
    node_pointer next = nullptr;

    while(node->position < key.size())
    {
        next = key.bit(node->position)? node->right : node->left;
        if(visitor(node, next)) break;
        node = next;
    }

    return node_pointer_pair(node, next);
}

template<typename KEY, typename Alloc>
void patricia_tree<KEY, Alloc>::recursive_traverse(node_pointer start_node, std::function<void(node_pointer)> visitor) {
    if(start_node->left != nullptr
            && start_node->left->position > start_node->position)
        recursive_traverse(start_node->left, visitor);

    if(start_node->right != nullptr
            && start_node->right->position > start_node->position)
        recursive_traverse(start_node->right, visitor);

    visitor(start_node);
}

template<typename KEY, typename Alloc>
void patricia_tree<KEY, Alloc>::remove_leaf(node_pointer parent, node_pointer node) {
    BitStream key(node->key);
    auto next_link = key.bit(node->position)? node->left : node->right;
    if(key.bit(parent->position)) {
        parent->right = next_link;
    } else {
        parent->left  = next_link;
    }

    //remove unlinked node
    node_allocator.destroy(node);
    node_allocator.deallocate(node, 1);
}

