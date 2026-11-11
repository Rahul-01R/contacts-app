#pragma once
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

// A hash table that maps a string key to a value of any type V.
// Example: HashTable<int> maps phone key -> contact id.
template <typename V>
class HashTable {
public:
    // Start with 53 empty buckets.
    HashTable() : buckets_(53) {}

    // djb2 hash: turns a string into one big non-negative number.
    // Public static so a test can check it.
    static std::size_t hashString(const std::string& s) {
        std::size_t h = 5381;
        for (char c : s) {
            h = h * 33 + static_cast<unsigned char>(c);
        }
        return h;
    }

    // Adds a key/value pair. Returns false (and changes nothing)
    // if the key already exists. Grows the table if it gets too full.
    bool insert(const std::string& key, const V& value) {
        std::vector<Entry>& bucket = buckets_[indexFor(key)];

        for (const Entry& e : bucket) {
            if (e.key == key) {
                return false;
            }
        }

        bucket.push_back(Entry{key, value});
        size_++;

        if (loadFactor() > 0.75) {
            rehash(nextCapacity(capacity()));
        }
        return true;
    }

    // Returns a pointer to the value, or nullptr if the key is missing.
    // The pointer is only valid until the next insert.
    V* find(const std::string& key) {
        for (Entry& e : buckets_[indexFor(key)]) {
            if (e.key == key) {
                return &e.value;
            }
        }
        return nullptr;
    }

    // Same, for a table that is const (read-only).
    const V* find(const std::string& key) const {
        for (const Entry& e : buckets_[indexFor(key)]) {
            if (e.key == key) {
                return &e.value;
            }
        }
        return nullptr;
    }

    // Removes a key. Returns true if something was removed.
    bool erase(const std::string& key) {
        std::vector<Entry>& bucket = buckets_[indexFor(key)];

        for (std::size_t i = 0; i < bucket.size(); i++) {
            if (bucket[i].key == key) {
                bucket[i] = bucket.back();  // overwrite with the last entry
                bucket.pop_back();          // remove the last entry
                size_--;
                return true;
            }
        }
        return false;
    }

    std::size_t size() const { return size_; }
    std::size_t capacity() const { return buckets_.size(); }
    std::size_t resizes() const { return resizes_; }

    double loadFactor() const {
        return static_cast<double>(size_) / static_cast<double>(buckets_.size());
    }

    // Length of the longest bucket list. Low means keys are well spread.
    std::size_t longestChain() const {
        std::size_t longest = 0;
        for (const std::vector<Entry>& bucket : buckets_) {
            if (bucket.size() > longest) {
                longest = bucket.size();
            }
        }
        return longest;
    }

    // Calls fn(key, value) once for every entry in the table.
    template <typename F>
    void forEach(F fn) const {
        for (const std::vector<Entry>& bucket : buckets_) {
            for (const Entry& e : bucket) {
                fn(e.key, e.value);
            }
        }
    }

private:
    // One stored item.
    struct Entry {
        std::string key;
        V value;
    };

    // buckets_[i] is the list of entries that hash to bucket i.
    std::vector<std::vector<Entry>> buckets_;
    std::size_t size_ = 0;
    std::size_t resizes_ = 0;

    // Which bucket does this key belong to?
    std::size_t indexFor(const std::string& key) const {
        return hashString(key) % buckets_.size();
    }

    // The next prime capacity bigger than the current one.
    static std::size_t nextCapacity(std::size_t current) {
        static const std::size_t primes[] = {53, 97, 193, 389, 769,
                                             1543, 3079, 6151, 12289, 24593};
        for (std::size_t p : primes) {
            if (p > current) {
                return p;
            }
        }
        return current * 2 + 1;  // list used up: keep growing anyway
    }

    // Builds a bigger bucket array and places every entry again.
    void rehash(std::size_t newCapacity) {
        std::vector<std::vector<Entry>> newBuckets(newCapacity);  // parentheses!

        for (const std::vector<Entry>& bucket : buckets_) {
            for (const Entry& e : bucket) {
                std::size_t i = hashString(e.key) % newCapacity;  // NEW capacity
                newBuckets[i].push_back(e);
            }
        }

        buckets_ = std::move(newBuckets);
        resizes_++;
    }
};