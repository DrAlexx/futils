#pragma once

#include <algorithm>
#include <iostream>
#include <cstddef>

namespace utils{

/**
 *
 */
template<class C>
class BitStreamAdaptor
{
public:
    typedef C value_type;
    typedef C& reference;
    typedef const C& const_reference;
    typedef typename  value_type::value_type item_type;
    typedef typename  value_type::value_type& item_reference;

    BitStreamAdaptor(const_reference v)
        :value(v){}

    /**
     * @brief size
     * @return data size in bites
     */
    std::size_t size() const
    {
        return value.size()*sizeof(item_type)*8;
    }

    /**
     * @brief bit
     * @param bit_pos bit position
     * @return true when bit by bit_pos is 1 otherwise false.
     * When bit_pos is great than data size also returns false
     */
    bool bit(size_t bit_pos) const
    {
        if(size() < bit_pos) return false;

        size_t i = bit_pos/(sizeof(item_type)*8);
        const auto& item = value.at(i);
        return item & (1<<(bit_pos%(sizeof(item_type)*8)));
    }

    /**
     * @brief mismatch
     * @param other const reference on another data
     * @return the first mismathed bit position
     */
    int mismatch(const_reference other) const
    {
        const auto& mismatch_item = std::mismatch(std::begin(value),
                                           std::end(value),
                                           std::begin(other));

        if(mismatch_item.first ==  std::end(value)
           || mismatch_item.second ==  std::end(other)) return -1;

        auto mask = *mismatch_item.first ^ *mismatch_item.second;

        //find on the first 1 in the mask
        unsigned i;
        for( i=0; i < sizeof(mask)*8; ++i){
            if(mask & (1<<i))break;
        }

        return i;
    }

private:
    const_reference value;
};

}//namespace utils

template <class C>
std::ostream &operator<<(std::ostream &output, const utils::BitStreamAdaptor<C> &stream)
{
    std::size_t size = stream.size();
    for(std::size_t i=0; i < size; ++i)
    {
        output << (stream.bit(i)? '1' : '0');
    }
    return output;
}


