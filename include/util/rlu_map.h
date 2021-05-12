#pragma once

#include <list>
#include <map>
#include <utility>

template<class T>
struct Weight{
    int operator()(T const& v){
        (void)v;
        return sizeof(T);
    }
};

/**
 * @class  rlu_map is
 * LRU = Last Recently Used
 */
template< typename Key
         , typename Value
         , typename Weight=Weight<Value>
         , typename Map=std::map<Key, std::pair<Value, typename std::list<Key>::iterator> >
        >
class rlu_map {
public:
    using key_type = Key;
    using value_type = Value;
    using reference  = value_type&;
    using const_reference =  const reference ;
    typedef typename std::list<key_type>::size_type size_type;
    typedef typename std::list<key_type>::iterator iterator;
    typedef typename std::list<key_type>::const_iterator const_iterator;
    typedef typename Map::iterator map_iterator;
    typedef typename Map::const_iterator map_const_iterator;

    rlu_map(int max_weight, float purge_factor=0.75f)
        :data_weight(0)
        ,maxWeight(max_weight)
        ,factor(purge_factor)
    {}

    void clear() {
        orderList.clear();
        map.clear();
        data_weight = 0;
    }

    iterator begin() {
        return orderList.begin();
    }

    const_iterator begin() const {
        return orderList.begin();
    }

    const_iterator cbegin() {
        return orderList.cbegin();
    }

    iterator end() {
        return orderList.end();
    }

    const_iterator end() const {
        return orderList.end();
    }

    const_iterator cend(){
        return orderList.cend();
    }

    bool empty() const {
        return map.empty();
    }

    size_type size() const {
        return map.size();
    }

    int weight() const {
        return data_weight;
    }

    map_const_iterator map_find(key_type k) {
        return map.find(k);
    }

    map_const_iterator map_end() {
        return map.end();
    }

    void touch(key_type k) {
        touch(list_find(k));
    }

    void touch(iterator i) {
        if(i != orderList.end()) {
            orderList.splice(orderList.begin(), orderList, i);
        }
    }

    void insert(key_type k, const_reference v, iterator position) {
        auto it = map_find(k);
        if(it == map.end()){
            Weight w;
            data_weight += w(v);
            auto list_it = orderList.insert(position, k);
            map.emplace(k, std::make_pair(v, list_it));
            purge();
        }
    }

    void push_front(key_type k, const_reference v) {
        insert(k, v, begin());
    }

    void push_back(key_type k, const_reference v) {
        return insert(k, v, end());
    }

    void remove(key_type k) {
        remove(list_find(k));
    }

    void remove(iterator i) {
        if(i != end()) {
            auto map_it = map.find(*i);
            Weight w;
            data_weight -= w(map_it->second.first);
            map.erase(map_it);
            orderList.erase(i);
        }
    }

private:
    //По ключу из списка ищем елемент в мапе и удаляем
    using KeyList = std::list<key_type>;
    KeyList orderList;
    //По итератору из мапы удаляем елемент списка
    Map map;
    int data_weight;
    int maxWeight;
    float factor;

    iterator list_find(key_type k) {
        auto it = map.find(k);
        return it == map.end()? orderList.end() : it->second.second;
    }

    void purge(){
        if(data_weight >= maxWeight){
            auto list_it = orderList.end();
            --list_it;
            while(data_weight > maxWeight*factor && !empty()) {
                auto remove_it = list_it;
                --list_it;
                remove(*remove_it);
            }
        }
    }
};
