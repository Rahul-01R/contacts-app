#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "../src/ContactBook.h"
#include "../src/Normalize.h"

Contact make(const std::string& name, const std::string& phone) {
    Contact c;
    c.name = name;
    c.phone = phone;
    return c;
}

bool sameList(const std::vector<Contact>& a, const std::vector<Contact>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); i++) {
        if (a[i].id != b[i].id || a[i].name != b[i].name ||
            a[i].phone != b[i].phone || a[i].email != b[i].email ||
            a[i].address != b[i].address) {
            return false;
        }
    }
    return true;
}

void check(const ContactBook& book) {
    std::string why;
    bool ok = book.verifyIndexes(&why);
    if (!ok) {
        std::cerr << "verifyIndexes failed: " << why << "\n";
    }
    assert(ok);
}

// The referee: a plain loop over every contact. No hash tables involved.
std::vector<Contact> bruteForceSearch(const ContactBook& book, const std::string& q) {
    std::vector<Contact> result;
    std::vector<Contact> all = book.listAll();   // already sorted by name

    std::string key = phoneKey(q);
    if (!key.empty()) {                          // a phone number
        for (const Contact& c : all) {
            if (phoneKey(c.phone) == key) {
                result.push_back(c);
            }
        }
        return result;
    }

    std::vector<std::string> queryWords = splitWords(q);
    if (queryWords.empty()) {
        return result;
    }
    for (const Contact& c : all) {
        std::vector<std::string> nameWords = splitWords(c.name);
        bool everyWordFound = true;
        for (const std::string& w : queryWords) {
            if (std::find(nameWords.begin(), nameWords.end(), w) == nameWords.end()) {
                everyWordFound = false;
                break;
            }
        }
        if (everyWordFound) {
            result.push_back(c);
        }
    }
    return result;
}

int main() {
    const std::vector<std::string> firsts = {"Asha", "Ravi", "Priya", "Aman",
                                             "Neha", "Jon", "John", "Isha"};
    const std::vector<std::string> lasts = {"Verma", "Rao", "Khan", "Das"};

    std::mt19937 rng(2024);                      // fixed seed: a failure repeats

    // The same phone written in three different ways.
    auto randomPhone = [&]() {
        std::string digits = std::to_string(9000000000LL + rng() % 60);   // only 60 numbers
        int style = rng() % 3;
        if (style == 1) {
            return "+91 " + digits;
        }
        if (style == 2) {
            return digits.substr(0, 5) + "-" + digits.substr(5);
        }
        return digits;
    };

    // Sometimes lowercase with extra spaces, to test the cleaning.
    auto randomName = [&]() {
        std::string name = firsts[rng() % firsts.size()] + " " + lasts[rng() % lasts.size()];
        if (rng() % 4 == 0) {
            name = "  " + name + "  ";
        }
        return name;
    };

    auto randomQuery = [&]() {
        int kind = rng() % 6;
        if (kind == 0) return firsts[rng() % firsts.size()];
        if (kind == 1) return lasts[rng() % lasts.size()];
        if (kind == 2) return firsts[rng() % firsts.size()] + " " + lasts[rng() % lasts.size()];
        if (kind == 3) return randomPhone();
        if (kind == 4) return std::string("zzz");
        return std::string("");
    };

    ContactBook book;
    std::vector<int> ids;                        // ids we believe exist
    int duplicates = 0;
    int warnings = 0;
    int rejectedEdits = 0;

    for (int step = 0; step < 2000; step++) {
        int op = rng() % 5;                      // more adds than removes

        if (op <= 1) {                           // add
            OpResult r = book.add(make(randomName(), randomPhone()));
            if (r.status == OpResult::OK) {
                ids.push_back(r.contact.id);
                if (!r.warnings.empty()) {
                    warnings++;
                }
            } else if (r.status == OpResult::DUPLICATE_PHONE) {
                duplicates++;
            }
        } else if (op <= 3 && !ids.empty()) {    // edit
            int id = ids[rng() % ids.size()];
            std::vector<Contact> before = book.listAll();
            OpResult r = book.edit(id, make(randomName(), randomPhone()));
            if (r.status != OpResult::OK) {
                rejectedEdits++;
                assert(sameList(before, book.listAll()));   // nothing may change
            }
        } else if (!ids.empty()) {               // remove
            std::size_t pos = rng() % ids.size();
            assert(book.remove(ids[pos]));
            ids.erase(ids.begin() + static_cast<std::ptrdiff_t>(pos));
        }

        check(book);
        assert(book.listAll().size() == ids.size());

        // Three random questions per step, fast answer versus referee.
        for (int k = 0; k < 3; k++) {
            std::string q = randomQuery();
            std::vector<Contact> fast = book.searchExact(q);
            std::vector<Contact> slow = bruteForceSearch(book, q);
            if (!sameList(fast, slow)) {
                std::cerr << "search disagrees for query '" << q << "' at step "
                          << step << ": fast " << fast.size() << ", slow "
                          << slow.size() << "\n";
            }
            assert(sameList(fast, slow));
        }
    }

    // Prove the interesting cases really happened.
    std::cout << "duplicate phones: " << duplicates
              << ", saves with warnings: " << warnings
              << ", rejected edits: " << rejectedEdits
              << ", contacts at the end: " << ids.size() << "\n";
    assert(duplicates > 0);
    assert(warnings > 0);
    assert(rejectedEdits > 0);

    std::cout << "ALL PASSED\n";
}