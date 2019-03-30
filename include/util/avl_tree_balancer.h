#pragma once

#include <stdint.h>
#include <type_traits>
#include <utility>

class AVLTreeBalancer {
public:
    struct NodeInfo {
        uint8_t height;
    };

    static int get_node_height(const NodeInfo* node) {
        if(node == nullptr)
            return 0;
        return node->height;
    }

    template <typename N, typename = typename std::enable_if<std::is_convertible<N,NodeInfo>::value>::type>
    static int get_balance_factor(const N* node) {
        if(node == nullptr)
            return 0;
        return get_node_height(node->links[0]) - get_node_height(node->links[1]);
    }

    template <typename N, typename = typename std::enable_if<std::is_convertible<N,NodeInfo>::value>::type>
    static void update_node_info(N* node) {
        if(node == nullptr)
            return;
        node->height = std::max(get_node_height(node->links[0]), get_node_height(node->links[1]))+1;
    }
};
