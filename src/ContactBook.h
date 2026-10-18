#pragma once
#include <cstddef>
#include <map>
#include <string>
#include <vector>
#include "Contact.h"
#include "HashTable.h"

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
    // The three structures. They must always describe the same set of contacts.
    std::map<int, Contact> byId_;               // id -> contact
    HashTable<int> phoneIndex_;                 // phone key -> contact id
    HashTable<std::vector<int>> nameIndex_;     // lowercase word -> contact ids
    int nextId_ = 1;

    // Checks the fields in order: name, phone, email, address.
    // Returns true if all are fine. Otherwise fills err (INVALID) and returns false.
    bool validate(const Contact& in, OpResult& err) const;

    // Puts a ready-to-store contact into all three structures.
    void store(const Contact& c);

    // Name-index helpers: add or remove this contact's id under each name word.
    void addNameWords(const Contact& c);
    void removeNameWords(const Contact& c);

    // Other contacts with the same name (ignoring capitals and extra spaces).
    std::vector<Warning> sameNameWarnings(const Contact& c) const;
};