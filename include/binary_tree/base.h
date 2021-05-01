#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <sstream>

namespace binary_tree {

template <typename Node>
Node* rotate_2(Node** path_top, int dir) noexcept {
    auto node_B = *path_top;
    auto node_D = node_B->links[dir];
    auto node_C = node_D->links[1 - dir];
    auto node_E = node_D->links[dir];

    *path_top = node_D;
    node_D->links[1 - dir] = node_B;
    node_B->links[dir]   = node_C;

    return node_E;
}

template <typename Node>
void rotate_3(Node** path_top, int dir) noexcept {
    auto node_B = *path_top;
    auto node_F = node_B->links[dir];
    auto node_D = node_F->links[1 - dir];
    /* note: C and E can be nullptr */
    auto node_C = node_D->links[1 - dir];
    auto node_E = node_D->links[dir];
    *path_top = node_D;
    node_D->links[1 - dir] = node_B;
    node_D->links[dir]     = node_F;
    node_B->links[dir]     = node_C;
    node_F->links[1 - dir] = node_E;
}

}
