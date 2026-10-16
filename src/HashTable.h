#pragma once
#include <cstddef>
#include <string>
#include <vector>

template <typename V>
class HashTable {
public:
    HashTable();
    bool insert(const std::string& key, const V& value);
    V* find(const std::string& key);
    const V* find(const std::string& key) const;
    bool erase(const std::string& key);
    std::size_t size() const;
    std::size_t capacity() const;
    double loadFactor() const;
    std::size_t longestChain() const;
    std::size_t resizes() const;

    template <typename F>
    void forEach(F fn) const;

    static std::size_t hashString(const std::string& s);

private:
    struct Entry { std::string key; V value; };
    std::vector<std::vector<Entry>> buckets_;
    std::size_t size_ = 0;
    std::size_t resizes_ = 0;

    std::size_t indexFor(const std::string& key) const;
    void rehash(std::size_t newCapacity);
    static std::size_t nextCapacity(std::size_t current);
};

template <typename V>
std::size_t HashTable<V>::hashString(const std::string& s) {
    std::size_t h = 5381;
    for (char c : s) {
        h = h * 33 + static_cast<unsigned char>(c);
    }
    return h;
}