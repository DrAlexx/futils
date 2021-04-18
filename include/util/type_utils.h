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

