#pragma once

#include <stdint.h>
#include <type_traits>
#include <utility>

class AVLTreeBalancer {
public:
    class NodeInfo {
    public:
        //Possible values:
        // -1 : tree is balanced
        //  1 : left tree is higher
        //  0 : right tree is higher
        int8_t balance = -1;

    public:
        auto get_balance() const { return balance; }
        bool is_balanced() const { return balance < 0; }
    };
};
