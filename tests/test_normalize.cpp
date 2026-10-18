#include <cassert>
#include <iostream>
#include "../src/Normalize.h"

int main() {
    // phoneKey: the seven rows of the plan's table
    assert(phoneKey("9876543210") == "9876543210");
    assert(phoneKey("+91 98765 43210") == "9876543210");
    assert(phoneKey("098765-43210") == "9876543210");
    assert(phoneKey("0091 98765 43210") == "9876543210");
    assert(phoneKey("98765 4321") == "");
    assert(phoneKey("call me") == "");
    assert(phoneKey("1234567890123456") == "");

    // trim and collapseSpaces
    assert(trim("  Asha  ") == "Asha");
    assert(trim("   ") == "");
    assert(collapseSpaces("Asha    Verma") == "Asha Verma");
    assert(normalizeName("  Asha   Verma ") == "Asha Verma");

    // toLowerAscii
    assert(toLowerAscii("ASHA Verma") == "asha verma");

    // splitWords
    std::vector<std::string> w = splitWords("Mary-Anne O'Neil");
    assert(w.size() == 4);
    assert(w[0] == "mary");
    assert(w[1] == "anne");
    assert(w[2] == "o");
    assert(w[3] == "neil");
    assert(splitWords("Asha asha").size() == 1);   // distinct words only
    assert(splitWords("  ").empty());

    // validEmail
    assert(validEmail(""));
    assert(validEmail("asha@example.com"));
    assert(!validEmail("abc"));
    assert(!validEmail("a@b"));
    assert(!validEmail("a@.com"));
    assert(!validEmail("a@b."));
    assert(!validEmail("@b.com"));
    assert(!validEmail("a b@c.com"));
    assert(!validEmail("a@@b.com"));

    std::cout << "ALL PASSED\n";
}