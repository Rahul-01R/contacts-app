#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "Contact.h"

inline void to_json(nlohmann::json& j, const Contact& c) {
    j = nlohmann::json{
        {"id", c.id},
        {"name", c.name},
        {"phone", c.phone},
        {"email", c.email},
        {"address", c.address}
    };
}

inline void from_json(const nlohmann::json& j, Contact& c) {
    c.id      = j.value("id", 0);
    c.name    = j.value("name", std::string());
    c.phone   = j.value("phone", std::string());
    c.email   = j.value("email", std::string());
    c.address = j.value("address", std::string());
}

std::vector<Contact> loadContacts(const std::string& path);
bool saveContacts(const std::string& path, const std::vector<Contact>& list);