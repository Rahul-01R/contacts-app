#include <cassert>
#include <iostream>
#include "../src/Normalize.h"

int main() {
    assert(phoneKey("9876543210") == "9876543210");
    assert(phoneKey("+91 98765 43210") == "9876543210");
    assert(phoneKey("098765-43210") == "9876543210");
    assert(phoneKey("0091 98765 43210") == "9876543210");
    assert(phoneKey("98765 4321") == "");          // 9 digits
    assert(phoneKey("call me") == "");             // no digits
    assert(phoneKey("1234567890123456") == "");    // 16 digits

    std::cout << "ALL PASSED\n";
}