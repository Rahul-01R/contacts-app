#include "ContactBook.h"

#include <algorithm>
#include <iostream>
#include "Normalize.h"

namespace {

// A copy of the contact with a cleaned name, email and address.
Contact cleanedCopy(const Contact& in) {
    Contact c = in;
    c.name = normalizeName(in.name);
    c.email = trim(in.email);
    c.address = trim(in.address);
    return c;
}

// Sorts by lowercase name; if names tie, by id.
void sortByName(std::vector<Contact>& list) {
    std::sort(list.begin(), list.end(), [](const Contact& a, const Contact& b) {
        std::string x = toLowerAscii(a.name);
        std::string y = toLowerAscii(b.name);
        if (x != y) {
            return x < y;
        }
        return a.id < b.id;
    });
}

}  // namespace

// ---------- private helpers ----------

bool ContactBook::validate(const Contact& in, OpResult& err) const {
    std::string name = normalizeName(in.name);
    if (name.empty()) {
        err.status = OpResult::INVALID;
        err.field = "name";
        err.message = "Name is required";
        return false;
    }
    if (name.size() > 100) {
        err.status = OpResult::INVALID;
        err.field = "name";
        err.message = "Name must be 100 characters or fewer";
        return false;
    }
    if (phoneKey(in.phone).empty()) {
        err.status = OpResult::INVALID;
        err.field = "phone";
        err.message = "Enter a phone number with at least 10 digits";
        return false;
    }
    if (!validEmail(trim(in.email))) {
        err.status = OpResult::INVALID;
        err.field = "email";
        err.message = "Enter a valid email like name@example.com";
        return false;
    }
    if (trim(in.address).size() > 300) {
        err.status = OpResult::INVALID;
        err.field = "address";
        err.message = "Address must be 300 characters or fewer";
        return false;
    }
    return true;
}

void ContactBook::store(const Contact& c) {
    byId_[c.id] = c;
    phoneIndex_.insert(phoneKey(c.phone), c.id);
    addNameWords(c);
}

void ContactBook::addNameWords(const Contact& c) {
    for (const std::string& word : splitWords(c.name)) {
        std::vector<int>* ids = nameIndex_.find(word);
        if (ids != nullptr) {
            ids->push_back(c.id);               // word already known: add the id
        } else {
            std::vector<int> fresh;             // new word: start a new list
            fresh.push_back(c.id);
            nameIndex_.insert(word, fresh);
        }
    }
}

void ContactBook::removeNameWords(const Contact& c) {
    for (const std::string& word : splitWords(c.name)) {
        std::vector<int>* ids = nameIndex_.find(word);
        if (ids == nullptr) {
            continue;
        }
        ids->erase(std::remove(ids->begin(), ids->end(), c.id), ids->end());
        if (ids->empty()) {
            nameIndex_.erase(word);             // no contact uses this word any more
        }
    }
}

std::vector<Warning> ContactBook::sameNameWarnings(const Contact& c) const {
    std::vector<Warning> result;

    std::vector<std::string> words = splitWords(c.name);
    if (words.empty()) {
        return result;
    }

    // Anyone with the same full name also has the first word in their name.
    const std::vector<int>* candidates = nameIndex_.find(words[0]);
    if (candidates == nullptr) {
        return result;
    }

    std::string mine = toLowerAscii(normalizeName(c.name));
    for (int id : *candidates) {
        if (id == c.id) {
            continue;                           // that is the contact itself
        }
        const Contact& other = byId_.at(id);
        if (toLowerAscii(normalizeName(other.name)) == mine) {
            Warning w;
            w.type = "same_name";
            w.other = other;
            result.push_back(w);
        }
    }
    return result;
}

// ---------- public functions ----------

void ContactBook::loadAll(const std::vector<Contact>& contacts) {
    // Start from a clean book.
    byId_.clear();
    phoneIndex_ = HashTable<int>();
    nameIndex_ = HashTable<std::vector<int>>();
    nextId_ = 1;

    int largestId = 0;
    for (const Contact& c : contacts) {
        if (c.id <= 0 || byId_.count(c.id) > 0) {
            std::cerr << "Skipping a contact with a bad or repeated id: " << c.id << "\n";
            continue;
        }

        OpResult check;
        if (!validate(c, check)) {
            std::cerr << "Skipping contact " << c.id << ": " << check.message << "\n";
            continue;
        }

        Contact saved = cleanedCopy(c);
        if (phoneIndex_.find(phoneKey(saved.phone)) != nullptr) {
            std::cerr << "Skipping contact " << c.id << ": phone number already used\n";
            continue;
        }

        store(saved);
        if (saved.id > largestId) {
            largestId = saved.id;
        }
    }
    nextId_ = largestId + 1;
}

OpResult ContactBook::add(const Contact& in) {
    OpResult result;

    // 1. Validate. On failure nothing has changed.
    if (!validate(in, result)) {
        return result;
    }

    // 2. Duplicate phone? One hash lookup.
    std::string key = phoneKey(in.phone);
    const int* ownerId = phoneIndex_.find(key);
    if (ownerId != nullptr) {
        result.status = OpResult::DUPLICATE_PHONE;
        result.contact = byId_.at(*ownerId);    // tell the caller who owns it
        return result;
    }

    // 3. Build the contact to save, with a new id.
    Contact saved = cleanedCopy(in);
    saved.id = nextId_;
    nextId_++;

    // 4. Same-name warnings (checked before storing, so it can't find itself).
    result.warnings = sameNameWarnings(saved);

    // 5. Store in all three structures.
    store(saved);

    result.status = OpResult::OK;
    result.contact = saved;
    return result;
}

// TEMPORARY until Day 5: finds the contact but changes nothing.
OpResult ContactBook::edit(int id, const Contact&) {
    OpResult result;
    Contact existing;
    if (!get(id, existing)) {
        result.status = OpResult::NOT_FOUND;
        return result;
    }
    result.status = OpResult::OK;
    result.contact = existing;
    return result;
}

bool ContactBook::remove(int id) {
    auto it = byId_.find(id);
    if (it == byId_.end()) {
        return false;
    }

    // Use the contact BEFORE erasing it from byId_.
    phoneIndex_.erase(phoneKey(it->second.phone));
    removeNameWords(it->second);
    byId_.erase(it);
    return true;
}

bool ContactBook::get(int id, Contact& out) const {
    auto it = byId_.find(id);
    if (it == byId_.end()) {
        return false;
    }
    out = it->second;
    return true;
}

std::vector<Contact> ContactBook::listAll() const {
    std::vector<Contact> list;
    for (const auto& pair : byId_) {
        list.push_back(pair.second);
    }
    sortByName(list);
    return list;
}

std::vector<Contact> ContactBook::searchExact(const std::string& q) const {
    std::vector<Contact> result;

    // A phone number in any format: one lookup in the phone index.
    std::string key = phoneKey(q);
    if (!key.empty()) {
        const int* id = phoneIndex_.find(key);
        if (id != nullptr) {
            result.push_back(byId_.at(*id));
        }
        return result;
    }

    // Otherwise treat q as name words. Every word must match.
    std::vector<std::string> words = splitWords(q);
    if (words.empty()) {
        return result;
    }

    const std::vector<int>* first = nameIndex_.find(words[0]);
    if (first == nullptr) {
        return result;
    }
    std::vector<int> ids = *first;              // a copy we can shrink

    for (std::size_t w = 1; w < words.size(); w++) {
        const std::vector<int>* list = nameIndex_.find(words[w]);
        if (list == nullptr) {
            return result;                      // one word has no match: nothing matches
        }
        std::vector<int> kept;
        for (int id : ids) {
            if (std::find(list->begin(), list->end(), id) != list->end()) {
                kept.push_back(id);             // id is in both lists
            }
        }
        ids = kept;
    }

    for (int id : ids) {
        result.push_back(byId_.at(id));
    }
    sortByName(result);
    return result;
}

std::vector<Contact> ContactBook::candidatesForFuzzy(const std::string&) const {
    return listAll();   // version 1: every contact is a candidate
}

BookStats ContactBook::stats() const {
    BookStats s;
    s.contacts = byId_.size();
    s.capacity = phoneIndex_.capacity();
    s.loadFactor = phoneIndex_.loadFactor();
    s.longestChain = phoneIndex_.longestChain();
    s.resizes = phoneIndex_.resizes();
    return s;
}