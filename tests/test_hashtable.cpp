#include <cassert>
#include <iostream>
#include <string>
#include "../src/HashTable.h"

int main() {
    // Test 1: known answer
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
        HashTable<int> t;
        t.insert(a, 10);
        t.insert(b, 20);
        assert(*t.find(a) == 10);
        assert(*t.find(b) == 20);
    }

    std::cout << "ALL PASSED\n";
}