#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>

/**
 * Provides member typedef 'type', which:
 *  is uint8_t when N <= 'uint8_t max value'
 *  is uint16_t when 'uint8_t max value'  < N <= 'uint16_t max value'
 *  is uint32_t when 'uint16_t max value' < N <= 'uint32_t max value'
 *  is uint64_t when 'uint32_t max value' < N
 */
template <std::size_t N>
using shrink_to_minimal_unsigned = std::conditional<N <= std::numeric_limits<uint8_t>::max(),  uint8_t,
                                   typename std::conditional<N <= std::numeric_limits<uint16_t>::max(), uint16_t,
                                   typename std::conditional<N <= std::numeric_limits<uint32_t>::max(), uint32_t,
                                   uint64_t>::type>::type>;

template<typename X>
struct is_reference_wrapper : std::false_type {
    using value_type = X;
};
 
template<typename X>
struct is_reference_wrapper<std::reference_wrapper<X>> : std::true_type {
    using value_type = typename std::reference_wrapper<X>::type;
};
 
template <typename X>
constexpr typename std::reference_wrapper<X>::type& get_reference(std::reference_wrapper<X> data) {
    return data.get();
}

template <typename X>
constexpr X& get_reference(X& data) {
    return data;
}
