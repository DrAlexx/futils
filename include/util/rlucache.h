#pragma once

#include <list>
#include <unordered_map>
#include <iterator>

template<class T>
struct Weight{
    int operator()(T const& /*v*/){
        return sizeof(T);
    }
};

template<class Value, typename Weight=Weight<Value>, typename Map=std::unordered_map<Value, typename std::list<Value>::iterator> >
class RLUCache{
    typedef Value value_type;
    typedef value_type& reference;
    typedef const reference const_reference;
    typedef value_type* pointer;
    typedef const pointer const_pointer;
    typedef typename Map::difference_type difference_type;
    typedef typename Map::size_type size_type;

    class iterator : public std::forward_iterator_tag {
        explicit iterator(typename Map::iterator it)
            :map_it(it){}

        explicit iterator(iterator& it)
            :map_it(it.map_it){}

        bool operator==(const iterator& it){
            return map_it == it.map_it;
        }

        bool operator!=(const iterator& it){
            return map_it != it.map_it;
        }

        reference operator*(){
            return (*map_it).first;
        }

        reference operator->(){
            return (*map_it).first;
        }

        iterator& operator++(){
            ++map_it;
            return *this;
        }

        private:
            typename Map::iterator map_it;
    };

    typedef const iterator const_iterator;

    RLUCache(int max_weight, float purge_factor=1.0f)
        :data_weight(0)
        ,maxWeight(max_weight)
        ,factor(purge_factor)
    {}

    void clear(){
        orderList.clear();
        collection.clear();
        data_weight = 0;
    }

    iterator begin(){
        return iterator(collection.begin());
    }

    const_iterator begin()const{
        return iterator(collection.begin());
    }

    const_iterator cbegin(){
        return iterator(collection.cbegin());
    }

    iterator end(){
        return iterator(collection.end());
    }

    const_iterator end()const{
        return iterator(collection.end());
    }

    const_iterator cend(){
        return iterator(collection.cend());
    }

    bool empty()const{
        return collection.empty();
    }

    size_type size()const{
        return collection.size();
    }

    int weight()const{
        return data_weight;
    }

    iterator find(const_reference v){
        return iterator(collection.find(v));
    }

    void touch(const_reference v){
        auto it = collection.find(v);
        touch(it);
    }

    void touch(const_iterator i){
        if(i != end()){
            orderList.splice(orderList.begin(), orderList, i->second);
        }
    }

    void insert(const_reference v){
        auto it = collection.find(v);
        if(it == collection.end()){
            weight += Weight(v);
            auto list_it = orderList.insert(orderList.begin(), v);
            collection.emplace(v, list_it);
            purge();
        }
    }

    void remove(const_reference v){
        auto it = collection.find(v);
        remove(it);
    }

    void remove(const_iterator i){
        if(i != end()){
            weight -= Weight(i->first);
            orderList.erase(i->second);
            collection.erase(i);
        }
    }

private:
    typedef std::list<Value>    ValueList;
    ValueList orderList;
    Map collection;
    int data_weight;
    int maxWeight;
    float factor;

    void purge(){
        if(data_weight >= maxWeight){
            auto list_it = orderList.rbegin();
            while(data_weight >maxWeight*factor
                  && !empty()){
                auto next = ++list_it;
                remove(*list_it);
                list_it = next;
            }
        }
    }
};
