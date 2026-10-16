// STUB: fake data so the server can be built. Replaced with real code on Day 4.
#include "ContactBook.h"

void ContactBook::loadAll(const std::vector<Contact>&) {
    // stub: does nothing
}

OpResult ContactBook::add(const Contact& in) {
    OpResult r;
    r.status = OpResult::OK;
    r.contact = in;
    r.contact.id = 99;                 // fake id
    return r;
}

OpResult ContactBook::edit(int id, const Contact& in) {
    OpResult r;
    r.status = OpResult::OK;
    r.contact = in;
    r.contact.id = id;
    return r;
}

bool ContactBook::remove(int) {
    return false;
}

bool ContactBook::get(int, Contact&) const {
    return false;
}

std::vector<Contact> ContactBook::listAll() const {
    std::vector<Contact> list;

    Contact a;
    a.id = 1;
    a.name = "Asha Verma";
    a.phone = "+91 98765 43210";
    a.email = "asha@example.com";
    a.address = "12 MG Road, Pune";
    list.push_back(a);

    Contact b;
    b.id = 2;
    b.name = "Ravi Verma";
    b.phone = "9123456780";
    list.push_back(b);

    return list;
}

std::vector<Contact> ContactBook::searchExact(const std::string&) const {
    return listAll();
}

std::vector<Contact> ContactBook::candidatesForFuzzy(const std::string&) const {
    return listAll();
}

BookStats ContactBook::stats() const {
    BookStats s;
    s.contacts = 2;
    s.capacity = 53;
    s.loadFactor = 0.04;
    s.longestChain = 1;
    s.resizes = 0;
    return s;
}