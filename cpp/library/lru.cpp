#include <iostream>
#include <list>
#include <unordered_map>

template <typename K, typename V> class LRUCache
{
  public:
    LRUCache(int capacity) : capacity_(capacity)
    {
    }

    V get(const K &key)
    {
        if (cacheMap_.find(key) == cacheMap_.end())
        {
            return V();
        }

        useList_.splice(useList_.begin(), useList_, cacheMap_[key]);
        return cacheMap_[key]->second;
    }

    void put(const K &key, const V &val)
    {
        if (cacheMap_.find(key) != cacheMap_.end())
        {
            cacheMap_[key]->second = val;
            useList_.splice(useList_.begin(), useList_, cacheMap_[key]);
        }
        else
        {
            if (useList_.size() == capacity_)
            {
                K lruKey = useList_.back().first;
                useList_.pop_back();
                cacheMap_.erase(lruKey);
            }
            useList_.emplace_front(key, val);
            cacheMap_[key] = useList_.begin();
        }
    }

  private:
    int capacity_;
    std::list<std::pair<K, V>> useList_;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> cacheMap_;
};

int main()
{
    LRUCache<int, int> lruCache(2);
    lruCache.put(1, 1);
    lruCache.put(2, 2);
    std::cout << lruCache.get(1) << std::endl;
    lruCache.put(3, 3);
    std::cout << lruCache.get(2) << std::endl;
    std::cout << lruCache.get(3) << std::endl;
}