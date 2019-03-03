
#pragma once

#include <atomic>
#include <mutex>
#include <vector>

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/trie_policy.hpp>

template <typename K>
std::size_t get_shard_index(K key, std::size_t N) {
    return static_cast<std::size_t>(key) % N;
}

std::size_t get_shard_index(const std::string& key, std::size_t N) {
    return static_cast<std::size_t>(*key.rbegin()) % N;
}

/**
 * @brief ShardMap
 */
template <typename K, typename V, typename M = __gnu_pbds::tree<K,V>>
class ShardMap {
public:
    using key_type    = typename M::key_type;
    using mapped_type = typename M::mapped_type;
    using value_type  = typename M::value_type;
    typedef M map_type;

    /**
     * @brief ShardMap Creates a ShardMap that has inside N shards.
     * @param N shard count. This number must be between 1 and 255.
     */
    explicit ShardMap(std::size_t N)
        : shards_array(N)
    {
        assert(N > 0 && N < 256);
    }

    /**
     * @brief insert a copy of the value in to map
     * @param v value that should be inserted
     */
    auto insert(const value_type& v) {
        auto& shard = get_shard(v.first);
        std::lock_guard lock(shard.mutex);
        auto& map = shard.map;
        auto result = map.insert(v).second;
        if (result)
            ++count;
        return result;
    }

    /**
     * @brief load is similar to insert except load is not thread safe.
     * Use it when you need to insert in the map a big amount of the data.
     * @param v value that should be inserted
     * @return bool true when the value was inserted. When the map already has a value with the same key returns false.
     */
    auto load(const value_type& v) {
        auto& shard = get_shard(v.first);
        auto& map = shard.map;
        auto result = map.insert(v).second;
        if (result)
            ++count;
        return result;
    }

    /**
     * @brief contains checks key is present in the map
     * @param key
     * @return bool true when the key is present in the map otherwise returns false.
     */
    auto contains(const key_type& key) const {
        auto& shard = get_shard(key);
        std::lock_guard lock(shard.mutex);
        auto& map = shard.map;
        return map.find(key) != map.end();
    }

    /**
     * @brief erase
     * @param key
     * @return
     */
    auto erase(const key_type& key) {
        auto& shard = get_shard(key);
        std::lock_guard lock(shard.mutex);
        auto& map = shard.map;
        auto result = map.erase(key);
        if (result)
            --count;
        return result;
    }

    /**
     * @brief at Access element
     * Returns a copy of the mapped value of the element identified with key key.
     * If 'key' does not match the key of any element in the container, the function throws an out_of_range exception.
     * @param Key value of the element whose mapped value is accessed.
     * Member type key_type is the type of the keys for the elements in the container, defined in map as an alias of its first template parameter (K).
     * @return A copy of the mapped value of the element with a key value equivalent to 'key'.
     * Member type mapped_type is the type to the mapped values in the container (see map member types). In map this is an alias of its second template parameter (V).
     */
    mapped_type at(const key_type& key) const {
        auto& shard = get_shard(key);
        std::lock_guard lock(shard.mutex);
        auto& map = shard.map;;
        auto it = map.find(key);
        if (it == map.end()) {
            throw std::out_of_range("No such key in the map");
        }
        return it->second;
    }

    /**
     * @brief update
     * @param key
     * @param updater
     */
    template<typename F>
    void update(const key_type& key, F updater) {
        auto& shard = get_shard(key);
        std::lock_guard lock(shard.mutex);
        auto& map = shard.map;
        auto it = map.find(key);
        if (it == map.end()) {
            throw std::out_of_range("No such key in the map");
        }
        updater(it->second);
    }

    /**
     * @brief update_each
     * @param updater
     */
    template<typename F>
    void update_each(F updater) {
        for(auto& shard : shards_array) {
            std::lock_guard lock(shard.mutex);
            auto& map = shard.map;
            std::for_each(map.begin(), map.end(),
                          updater);
        }
    }

    /**
     * @brief size Returns the number of elements in the map container.
     * @return The number of elements in the container.
     */
    size_t size() const noexcept {
        return count;
    }

    /**
     * @brief empty Test whether container is empty
     * Returns whether the map container is empty (i.e. whether its size is 0).
     * @return true if the container size is 0, false otherwise.
     */
    bool empty() const noexcept {
        return count == 0;
    }

private:
    struct Shard {
        mutable std::mutex  mutex;
        map_type            map;
    };
    std::vector<Shard>      shards_array;
    std::atomic_size_t      count = 0;

    Shard& get_shard(const key_type& key) {
        auto i = get_shard_index(key, shards_array.size());
        return shards_array[i];
    }
    const Shard& get_shard(const key_type& key) const {
        auto i = get_shard_index(key, shards_array.size());
        return shards_array[i];
    }
};

template <typename V>
using StringShardMap = ShardMap<std::string, V, __gnu_pbds::trie<std::string,V>>;
