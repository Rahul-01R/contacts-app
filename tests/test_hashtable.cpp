#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include "../src/HashTable.h"

int main() {
    // Test 1: the known answer from the plan
    assert(HashTable<int>::hashString("ab") == 5863208);
    assert(HashTable<int>::hashString("ab") % 53 == 30);

    // Test 2: three keys inserted, each found, size is 3
    {
        HashTable<int> t;
        assert(t.insert("one", 1));
        assert(t.insert("two", 2));
        assert(t.insert("three", 3));
        assert(t.size() == 3);
        assert(*t.find("one") == 1);
        assert(*t.find("two") == 2);
        assert(*t.find("three") == 3);
    }

    // Test 3: a missing key gives nullptr
    {
        HashTable<int> t;
        t.insert("one", 1);
        assert(t.find("zzz") == nullptr);
    }

    // Test 4: inserting an existing key returns false and keeps the old value
    {
        HashTable<int> t;
        assert(t.insert("one", 1));
        assert(!t.insert("one", 999));
        assert(*t.find("one") == 1);
        assert(t.size() == 1);
    }

    // Test 5: two different keys in the same bucket are both found
    {
        std::string a = "k0";
        std::string b;
        for (int i = 1; i < 1000; i++) {
            b = "k" + std::to_string(i);
            if (HashTable<int>::hashString(a) % 53 ==
                HashTable<int>::hashString(b) % 53) {
                break;
            }
        }
        assert(HashTable<int>::hashString(a) % 53 ==
               HashTable<int>::hashString(b) % 53);  // really a collision

        HashTable<int> t;
        t.insert(a, 10);
        t.insert(b, 20);
        assert(*t.find(a) == 10);
        assert(*t.find(b) == 20);
        assert(t.longestChain() == 2);
    }

    // Test 6: 1000 keys force rehashing, and nothing is lost
    {
        HashTable<int> t;
        for (int i = 0; i < 1000; i++) {
            assert(t.insert("key" + std::to_string(i), i));
        }
        for (int i = 0; i < 1000; i++) {
            int* p = t.find("key" + std::to_string(i));
            assert(p != nullptr);
            assert(*p == i);
        }
        assert(t.size() == 1000);
        assert(t.resizes() > 0);
        assert(t.loadFactor() <= 0.75);
    }

    // Test 7: erase every second key
    {
        HashTable<int> t;
        for (int i = 0; i < 1000; i++) {
            t.insert("key" + std::to_string(i), i);
        }
        for (int i = 0; i < 1000; i += 2) {
            assert(t.erase("key" + std::to_string(i)));
        }
        for (int i = 0; i < 1000; i++) {
            int* p = t.find("key" + std::to_string(i));
            if (i % 2 == 0) {
                assert(p == nullptr);   // erased
            } else {
                assert(p != nullptr);   // still there
                assert(*p == i);
            }
        }
        assert(t.size() == 500);
        assert(!t.erase("key0"));       // already gone
    }

    // Test 8: forEach visits every entry once
    {
        HashTable<int> t;
        for (int i = 0; i < 100; i++) {
            t.insert("key" + std::to_string(i), i);
        }
        int count = 0;
        long long sum = 0;
        t.forEach([&](const std::string&, const int& value) {
            count++;
            sum += value;
        });
        assert(count == 100);
        assert(sum == 4950);            // 0 + 1 + ... + 99
    }

    // Test 9: random operations, compared with std::unordered_map (the referee)
    {
        std::mt19937 rng(12345);        // fixed seed, so a failure repeats
        HashTable<int> mine;
        std::unordered_map<std::string, int> ref;

        for (int step = 0; step < 20000; step++) {
            std::string key = "key" + std::to_string(rng() % 500);
            int op = rng() % 3;

            if (op == 0) {              // insert
                int value = rng() % 1000;
                bool a = mine.insert(key, value);
                bool b = ref.insert({key, value}).second;
                assert(a == b);
            } else if (op == 1) {       // find
                int* p = mine.find(key);
                auto it = ref.find(key);
                assert((p != nullptr) == (it != ref.end()));
                if (p != nullptr) {
                    assert(*p == it->second);
                }
            } else {                    // erase
                bool a = mine.erase(key);
                bool b = (ref.erase(key) == 1);
                assert(a == b);
            }

            assert(mine.size() == ref.size());
        }
    }

    std::cout << "ALL PASSED\n";
}