#pragma once

#include <memory>

namespace util {

template <typename T>
class allocator {
  public:
    using value_type = T;
    using size_type  = std::size_t;
    using difference_type = std::ptrdiff_t;

    allocator() noexcept {}

    [[nodiscard]] constexpr T* allocate(std::size_t n, std::align_val_t align) {
        return static_cast<T*>(::operator new(n * sizeof(T), align));
    }

    constexpr void deallocate(T* p, std::align_val_t align) {
        ::operator delete(p, align);
    }
};
}
