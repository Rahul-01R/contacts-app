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

template <typename V>
HashTable<V>::HashTable() : buckets_(53) {}

template <typename V>
std::size_t HashTable<V>::indexFor(const std::string& key) const {
    return hashString(key) % buckets_.size();
}

template <typename V>
bool HashTable<V>::insert(const std::string& key, const V& value) {
    std::size_t i = indexFor(key);              // 1. which bucket?
    for (const Entry& e : buckets_[i]) {        // 2. walk that bucket's list
        if (e.key == key) {
            return false;                       //    key already there: refuse
        }
    }
    buckets_[i].push_back(Entry{key, value});   // 3. add to the end of the list
    size_++;                                    // 4. one more item in the table
    return true;
}
template <typename V>
V* HashTable<V>::find(const std::string& key) {
    std::size_t i = indexFor(key);
    for (Entry& e : buckets_[i]) {
        if (e.key == key) {
            return &e.value;        // address of the value
        }
    }
    return nullptr;                 // not found
}

template <typename V>
const V* HashTable<V>::find(const std::string& key) const {
    std::size_t i = indexFor(key);
    for (const Entry& e : buckets_[i]) {
        if (e.key == key) {
            return &e.value;
        }
    }
    return nullptr;
}
template <typename V>
std::size_t HashTable<V>::size() const { return size_; }

template <typename V>
std::size_t HashTable<V>::capacity() const { return buckets_.size(); }

template <typename V>
double HashTable<V>::loadFactor() const {
    return static_cast<double>(size_) / static_cast<double>(buckets_.size());
}
