#include <cassert>
#include <iostream>
#include "../src/HashTable.h"
#include<HashTable.h>
int main() {
    // Test 1: the known answer from the plan
    assert(HashTable<int>::hashString("ab") == 5863208);
    assert(HashTable<int>::hashString("ab") % 53 == 30);

    std::cout << "ALL PASSED\n";
}