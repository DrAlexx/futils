#pragma once

#include <list>
#include <unordered_map>
#include <utility>

template<class T>
struct Weight{
    int operator()(T const& /*v*/){
        return sizeof(T);
    }
};

template< class Key
         ,class Value
         ,typename Weight=Weight<Value>
         ,typename Map=std::unordered_map<Key, typename std::pair<Value, typename std::list<Key>::iterator> >
        >
class RLUCache{
public:
    typedef Key key_type;
    typedef typename Map::mapped_type mapped_type;
    typedef Value value_type;
    typedef value_type& reference;
    typedef const reference const_reference;
    typedef value_type* pointer;
    typedef const pointer const_pointer;
    typedef typename std::list<Value>::difference_type difference_type;
    typedef typename std::list<Value>::size_type size_type;
    typedef typename std::list<Value>::iterator iterator;
    typedef typename std::list<Value>::const_iterator const_iterator;

    RLUCache(int max_weight, float purge_factor=0.75f)
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
        return orderList.begin();
    }

    const_iterator begin()const{
        return orderList.begin();
    }

    const_iterator cbegin(){
        return orderList.cbegin();
    }

    iterator end(){
        return orderList.end();
    }

    const_iterator end()const{
        return orderList.end();
    }

    const_iterator cend(){
        return orderList.cend();
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

    iterator find(key_type k){
        auto it = collection.find(k);
        return it==collection.end()?
                    orderList.end()
                    : it->second.second;
    }

    void touch(key_type k){
        touch(find(k));
    }

    void touch(const_iterator i){
        if(i != orderList.end()){
            orderList.splice(orderList.begin(), orderList, i);
        }
    }

    void insert(key_type k, const_reference v, const_iterator position=begin()){
        auto it = collection.find(k);
        if(it == collection.end()){
            weight += Weight(v);
            auto list_it = orderList.insert(position, k);
            collection.emplace(k, std::make_pair(v,list_it));
            purge();
        }
    }

    void push_front(key_type k, const_reference v){
        insert(k,v);
    }

    void push_back(key_type k, const_reference v){
        return insert(k,v, end());
    }

    void remove(key_type k){
        remove(find(k));
    }

    void remove(const_iterator i){
        if(i != end()){
            weight -= Weight(i->first);
            collection.erase(*i);
            orderList.erase(i);
        }
    }

private:
    typedef std::list<key_type>    KeyList;
    KeyList orderList;
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
