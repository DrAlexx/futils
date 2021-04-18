#pragma once

#include "type_utils.h"

#include <cassert>
#include <limits>
#include <memory>
#include <stdexcept>
#include <stdint.h>
#include <type_traits>

template <typename T = uint8_t>
class ConstPoolBase {
public:
    using index_type = T;
    using size_type  = std::size_t;

    static_assert(std::is_unsigned_v<index_type>, "T must be unsigned");

    ConstPoolBase() = delete;
    ConstPoolBase(const ConstPoolBase&) = delete;
    ConstPoolBase(const ConstPoolBase&&) = delete;

    ConstPoolBase(size_type count, size_type element_size)
        : size(count)
        , element_sz(element_size)
        , data(new uint8_t[element_size * count])
    {
        assert(element_size >= sizeof(index_type));
        assert(count <= std::numeric_limits<index_type>::max());

        Reset();
    }

    size_type GetSize() const {
        return size;
    }

    size_type GetElementSize() const {
        return element_sz;
    }

    void Reset() noexcept {
        for (index_type i = 0; i < size; ++i) {
            get_index(i) = i + 1;
        }
        first_empty = 0;
    }

    std::pair<index_type, void*> Alloc() {
        if (first_empty >= size) {
            throw std::bad_alloc();
        }
        auto it = first_empty;
        void* ptr = get_element(first_empty);
        first_empty = *reinterpret_cast<index_type*>(ptr);
        return {it, ptr};
    }

    void Free(index_type i) {
        if (i >= size) {
            std::bad_alloc();
        }
        //TODO: check double free here
        get_index(i) = first_empty;
        first_empty = i;
    }

    void* GetAddr(index_type i) noexcept {
        return get_element(i);
    }

    void* GetAddrAt(index_type i) {
        if (i >= size)
            throw std::out_of_range("Index is to big");
        return GetAddr(i);
    }

private:
    size_type   size;
    size_type   element_sz;
    index_type  first_empty = 0;
    std::unique_ptr<uint8_t[]> data;

    void* get_element(index_type i) noexcept {
        return data.get() + element_sz * i;
    }

    index_type& get_index(index_type i) noexcept {
        index_type* index_ptr = reinterpret_cast<index_type*>(get_element(i));
        return *index_ptr;
    }
};

template <typename T, std::size_t N>
class ConstPool : public ConstPoolBase<typename shrink_to_minimal_unsigned<N>::type>
{
public:
    using base_class = ConstPoolBase<typename shrink_to_minimal_unsigned<N>::type>;
    using index_type = typename base_class::index_type;

    static_assert(N != 0, "Here is no sense!");
    static_assert(!std::is_void_v<T>, "Here is no sense!");
    static_assert(sizeof(T) >= sizeof(index_type), "sizeof(T) must be greater or equivalent to sizeof(index_type)");

    ConstPool()
        : base_class(N, sizeof(T))
    {}

    ConstPool(const ConstPool&) = delete;
    ConstPool(const ConstPool&&) = delete;

    std::pair<index_type, T*> Alloc() {
        auto result = base_class::Alloc();
        return {result.first, reinterpret_cast<T*>(result.second)};
    }

    T* GetAddr(index_type i) noexcept {
        return reinterpret_cast<T*>(base_class::GetAddr(i));
    }

    T* GetAddrAt(index_type i) {
        return reinterpret_cast<T*>(base_class::GetAddrAt(i));
    }
};
