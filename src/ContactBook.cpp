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
// A linear scan over all contacts is fine at this size (hundreds of contacts).
// A third hash table would be overkill, so we keep it simple.
std::vector<Warning> ContactBook::sameEmailWarnings(const Contact& c) const {
    std::vector<Warning> result;
    if (c.email.empty()) {
        return result;                          // no email, nothing to compare
    }

    std::string mine = toLowerAscii(c.email);
    for (const auto& pair : byId_) {
        const Contact& other = pair.second;
        if (other.id == c.id) {
            continue;                           // that is the contact itself
        }
        if (toLowerAscii(other.email) == mine) {
            Warning w;
            w.type = "same_email";
            w.other = other;
            result.push_back(w);
        }
    }
    return result;
}

std::vector<Warning> ContactBook::warningsFor(const Contact& c) const {
    std::vector<Warning> all = sameNameWarnings(c);
    std::vector<Warning> emailOnes = sameEmailWarnings(c);
    all.insert(all.end(), emailOnes.begin(), emailOnes.end());
    return all;
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
        result.warnings = warningsFor(saved);

    // 5. Store in all three structures.
    store(saved);

    result.status = OpResult::OK;
    result.contact = saved;
    return result;
}

OpResult ContactBook::edit(int id, const Contact& in) {
    OpResult result;

    // ----- CHECKS (nothing is changed in this part) -----

    // 1. Does the contact exist?
    auto it = byId_.find(id);
    if (it == byId_.end()) {
        result.status = OpResult::NOT_FOUND;
        return result;
    }

    // 2. Are the new values valid?
    if (!validate(in, result)) {
        return result;
    }

    // 3. Would the new phone clash with a DIFFERENT contact?
    std::string oldKey = phoneKey(it->second.phone);
    std::string newKey = phoneKey(in.phone);
    if (newKey != oldKey) {
        const int* ownerId = phoneIndex_.find(newKey);
        if (ownerId != nullptr) {
            result.status = OpResult::DUPLICATE_PHONE;
            result.contact = byId_.at(*ownerId);
            return result;
        }
    }

    // ----- CHANGES (every check has passed, so nothing can fail now) -----

    Contact oldContact = it->second;            // copy, needed to remove old name words
    Contact saved = cleanedCopy(in);
    saved.id = id;                              // the client's id is ignored

    // 4. Phone index: only touch it if the key really changed.
    if (newKey != oldKey) {
        phoneIndex_.erase(oldKey);
        phoneIndex_.insert(newKey, id);
    }

    // 5. Name index: remove the old words, add the new ones.
    removeNameWords(oldContact);
    addNameWords(saved);

    // 6. Main storage, then warnings (the contact skips itself in the checks).
    byId_[id] = saved;
    result.warnings = warningsFor(saved);
    result.status = OpResult::OK;
    result.contact = saved;
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
bool ContactBook::verifyIndexes(std::string* whyNot) const {
    // Small helper: store the reason (if the caller wants it) and return false.
    auto fail = [&](const std::string& why) {
        if (whyNot != nullptr) {
            *whyNot = why;
        }
        return false;
    };

    // Rule 2: both indexes and the main storage have the same number of contacts.
    if (phoneIndex_.size() != byId_.size()) {
        return fail("phoneIndex_ has " + std::to_string(phoneIndex_.size()) +
                    " entries but byId_ has " + std::to_string(byId_.size()));
    }

    // Rules 1 and 3: walk every contact and check what the indexes say about it.
    for (const auto& pair : byId_) {
        const Contact& c = pair.second;

        // Rule 1: the phone key points back to this contact.
        std::string key = phoneKey(c.phone);
        const int* owner = phoneIndex_.find(key);
        if (owner == nullptr) {
            return fail("contact " + std::to_string(c.id) +
                        " has phone key " + key + " missing from phoneIndex_");
        }
        if (*owner != c.id) {
            return fail("phone key " + key + " points to id " +
                        std::to_string(*owner) + " but belongs to id " +
                        std::to_string(c.id));
        }

        // Rule 3: every name word lists this id exactly once.
        for (const std::string& word : splitWords(c.name)) {
            const std::vector<int>* ids = nameIndex_.find(word);
            if (ids == nullptr) {
                return fail("contact " + std::to_string(c.id) +
                            ": word '" + word + "' is missing from nameIndex_");
            }
            int times = static_cast<int>(std::count(ids->begin(), ids->end(), c.id));
            if (times != 1) {
                return fail("word '" + word + "' lists id " + std::to_string(c.id) +
                            " " + std::to_string(times) + " times (should be 1)");
            }
        }
    }

    // Rule 4: nothing in nameIndex_ mentions a contact that does not exist.
    // forEach cannot "return false" for us, so the lambda records the first problem.
    std::string problem;
    nameIndex_.forEach([&](const std::string& word, const std::vector<int>& ids) {
        if (!problem.empty()) {
            return;                                   // already found one, skip the rest
        }
        if (ids.empty()) {
            problem = "word '" + word + "' has an empty id list (it should be erased)";
            return;
        }
        for (int id : ids) {
            auto it = byId_.find(id);
            if (it == byId_.end()) {
                problem = "word '" + word + "' lists id " + std::to_string(id) +
                          " which does not exist";
                return;
            }
            // Stricter check: the word must really be in that contact's name,
            // otherwise an edit left an old word behind.
            std::vector<std::string> words = splitWords(it->second.name);
            if (std::find(words.begin(), words.end(), word) == words.end()) {
                problem = "word '" + word + "' lists id " + std::to_string(id) +
                          " but that contact's name has no such word";
                return;
            }
        }
    });
    if (!problem.empty()) {
        return fail(problem);
    }

    return true;
}