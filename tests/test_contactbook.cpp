#include <cassert>
#include <iostream>
#include "../src/ContactBook.h"

// Small helper so each test stays short.
Contact make(const std::string& name, const std::string& phone,
             const std::string& email = "", const std::string& address = "") {
    Contact c;
    c.name = name;
    c.phone = phone;
    c.email = email;
    c.address = address;
    return c;
}

int main() {
    // Case 1: add a valid contact
    {
        ContactBook book;
        OpResult r = book.add(make("Asha Verma", "9876543210"));
        assert(r.status == OpResult::OK);
        assert(r.contact.id > 0);
        assert(book.listAll().size() == 1);
        Contact got;
        assert(book.get(r.contact.id, got));
        assert(got.name == "Asha Verma");
    }

    // Case 2: same phone in another format is a duplicate
    {
        ContactBook book;
        book.add(make("Asha Verma", "9876543210"));
        OpResult r = book.add(make("Someone Else", "+91 98765 43210"));
        assert(r.status == OpResult::DUPLICATE_PHONE);
        assert(r.contact.name == "Asha Verma");     // the existing owner
        assert(book.listAll().size() == 1);         // nothing was added
    }

    // Case 3: empty name
    {
        ContactBook book;
        OpResult r = book.add(make("   ", "9876543210"));
        assert(r.status == OpResult::INVALID);
        assert(r.field == "name");
        assert(book.listAll().empty());
    }

    // Case 4: 9-digit phone
    {
        ContactBook book;
        OpResult r = book.add(make("Asha", "987654321"));
        assert(r.status == OpResult::INVALID);
        assert(r.field == "phone");
    }

    // Case 5: bad email
    {
        ContactBook book;
        OpResult r = book.add(make("Asha", "9876543210", "abc"));
        assert(r.status == OpResult::INVALID);
        assert(r.field == "email");
    }

    // Case 6: same name, different phones: both saved, second one warns
    {
        ContactBook book;
        OpResult first = book.add(make("Asha Verma", "9876543210"));
        OpResult second = book.add(make("asha  VERMA", "9000000000"));
        assert(first.status == OpResult::OK);
        assert(first.warnings.empty());
        assert(second.status == OpResult::OK);
        assert(second.warnings.size() == 1);
        assert(second.warnings[0].type == "same_name");
        assert(second.warnings[0].other.phone == "9876543210");
        assert(book.listAll().size() == 2);
    }

    // Case 11: remove a contact, then add the same phone again
    {
        ContactBook book;
        OpResult r = book.add(make("Asha Verma", "9876543210"));
        int id = r.contact.id;
        assert(book.remove(id));
        Contact got;
        assert(!book.get(id, got));
        assert(book.searchExact("asha").empty());
        assert(book.searchExact("9876543210").empty());
        OpResult again = book.add(make("Asha Verma", "9876543210"));
        assert(again.status == OpResult::OK);
    }

    // Case 12: remove an id that does not exist
    {
        ContactBook book;
        assert(!book.remove(12345));
    }

    // Case 13: listAll is sorted by lowercase name
    {
        ContactBook book;
        book.add(make("Chitra", "9000000003"));
        book.add(make("asha", "9000000001"));
        book.add(make("Bala", "9000000002"));
        std::vector<Contact> list = book.listAll();
        assert(list.size() == 3);
        assert(list[0].name == "asha");
        assert(list[1].name == "Bala");
        assert(list[2].name == "Chitra");
    }

    // Case 14: searchExact
    {
        ContactBook book;
        book.add(make("Asha Verma", "9876543210"));
        book.add(make("Ravi Verma", "9123456780"));
        book.add(make("Priya Rao", "9000000009"));

        std::vector<Contact> r = book.searchExact("asha verma");
        assert(r.size() == 1);
        assert(r[0].name == "Asha Verma");

        assert(book.searchExact("VERMA").size() == 2);
        assert(book.searchExact("ash").empty());        // exact only
        assert(book.searchExact("+91 98765 43210").size() == 1);
        assert(book.searchExact("98765-43210").size() == 1);
        assert(book.searchExact("asha rao").empty());   // both words must match
    }

    // Extra: loadAll skips bad data and sets the next id
    {
        std::vector<Contact> data;
        Contact a = make("Asha Verma", "9876543210");
        a.id = 5;
        Contact b = make("Ravi Verma", "9123456780");
        b.id = 7;
        Contact dup = make("Copy", "+91 9876543210");   // phone of Asha
        dup.id = 8;
        Contact bad = make("", "9000000000");           // empty name
        bad.id = 9;
        data.push_back(a);
        data.push_back(b);
        data.push_back(dup);
        data.push_back(bad);

        ContactBook book;
        book.loadAll(data);
        assert(book.listAll().size() == 2);
        OpResult r = book.add(make("New Person", "9111111111"));
        assert(r.contact.id == 8);                      // largest id (7) + 1
    }

    // Extra: stats reflect the phone index
    {
        ContactBook book;
        for (int i = 0; i < 100; i++) {
            book.add(make("Person " + std::to_string(i),
                          std::to_string(9000000000LL + i)));
        }
        BookStats s = book.stats();
        assert(s.contacts == 100);
        assert(s.resizes > 0);
        assert(s.loadFactor <= 0.75);
    }

    std::cout << "ALL PASSED\n";
}