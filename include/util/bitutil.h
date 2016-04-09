#pragma once

namespace utils{

/**
 *
 */
template<class C>
class BitStreamAdaptor
{
public:
    typedef C value_type;
    typedef typename value_type& reference;
    typedef const reference const_reference;
    typedef typename  value_type::value_type item_type;
    typedef typename  item_type& item_reference;
    typedef const item_reference const_item_reference;

    BitStreamAdaptor(const_reference v)
        :value(v){}

    /**
     * @brief size
     * @return data size in bites
     */
    size_t size() const
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
        const auto& item = container.at(i);
        return item && (1<<(bit_pos%(sizeof(item_type)*8)));
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
        int i;
        for( i=0; i < sizeof(mask)*8; ++i){
            if(mask & (1<<i))break;
        }

        return i;
    }

private:
    const_reference value;
};

}//namespace util
