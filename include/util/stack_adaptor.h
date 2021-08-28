#pragma once

#include  <stdexcept>
#include <span>

namespace util {

template <typename T>
class stack_adaptor {
public:
    using value_type = T;

    stack_adaptor()
    {}

    stack_adaptor(std::span<T> s)
        : span(s)
    {}

    void set_buffer(std::span<T> s) {
        span = s;
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return head == 0;
    }

    value_type& front() const {
        return span[head - 1];
    }

    void push(value_type v) {
//        if (head == span.size()) {
//            throw std::runtime_error("OOps!");
//        }
        span[head++] = v;
    }

    value_type pop() {
        auto v = span[head - 1];
        head--;
        return v;
    }

private:
    std::span<T> span;
    std::size_t  head = 0;
};

}
