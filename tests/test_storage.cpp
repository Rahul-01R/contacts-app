#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include "../src/Storage.h"

int main() {
    const std::string testFile = "data/test_temp_contacts.json";
    const std::string corruptFile = testFile + ".corrupt";
    std::remove(testFile.c_str());
    std::remove(corruptFile.c_str());

    // 1. Missing file returns empty vector
    auto emptyList = loadContacts(testFile);
    assert(emptyList.empty());

    // 2. Save and load round-trip
    std::vector<Contact> original = {
        {1, "Asha Verma", "+91 98765 43210", "asha@example.com", "12 MG Road, Pune"},
        {2, "Ravi Sharma", "9123456780", "", ""}
    };
    assert(saveContacts(testFile, original));

    auto loaded = loadContacts(testFile);
    assert(loaded.size() == 2);
    assert(loaded[0].id == 1);
    assert(loaded[0].name == "Asha Verma");
    assert(loaded[0].phone == "+91 98765 43210");
    assert(loaded[0].email == "asha@example.com");
    assert(loaded[0].address == "12 MG Road, Pune");
    assert(loaded[1].id == 2);
    assert(loaded[1].name == "Ravi Sharma");

    // 3. Broken JSON is not overwritten and is renamed to .corrupt
    {
        std::ofstream bad(testFile);
        bad << "{ broken json ";
    }
    auto fromCorrupt = loadContacts(testFile);
    assert(fromCorrupt.empty());

    std::ifstream checkCorrupt(corruptFile);
    assert(checkCorrupt.is_open());
    checkCorrupt.close();

    std::remove(testFile.c_str());
    std::remove(corruptFile.c_str());

    std::cout << "ALL PASSED" << std::endl;
    return 0;
}