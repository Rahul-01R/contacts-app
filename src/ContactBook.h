#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "Contact.h"

struct Warning {                   // saved, but the user should know
    std::string type;              // "same_name" or "same_email"
    Contact other;                 // the other contact involved
};

struct OpResult {
    enum Status { OK, INVALID, DUPLICATE_PHONE, NOT_FOUND };
    Status status = OK;
    Contact contact;               // OK: saved contact. DUPLICATE_PHONE: the existing owner
    std::string field;             // INVALID only: "name", "phone", "email" or "address"
    std::string message;           // INVALID only: a sentence for the user
    std::vector<Warning> warnings; // OK only
};

struct BookStats {
    std::size_t contacts = 0;
    std::size_t capacity = 0;      // of the phone index
    double loadFactor = 0;
    std::size_t longestChain = 0;
    std::size_t resizes = 0;
};

class ContactBook {
public:
    void loadAll(const std::vector<Contact>& contacts);
    OpResult add(const Contact& in);
    OpResult edit(int id, const Contact& in);
    bool remove(int id);
    bool get(int id, Contact& out) const;
    std::vector<Contact> listAll() const;
    std::vector<Contact> searchExact(const std::string& q) const;
    std::vector<Contact> candidatesForFuzzy(const std::string& q) const;
    BookStats stats() const;

private:
    // Day 4: the real data structures go here.
};