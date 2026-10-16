#include "Storage.h"
#include <cstdio>
#include <fstream>
#include <iostream>

std::vector<Contact> loadContacts(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        return {};
    }

    try {
        nlohmann::json j;
        in >> j;
        in.close();

        if (!j.is_array()) {
            std::cerr << "Error: " << path << " is not a JSON array. Renaming to .corrupt\n";
            std::string corruptPath = path + ".corrupt";
            std::remove(corruptPath.c_str());
            std::rename(path.c_str(), corruptPath.c_str());
            return {};
        }

        return j.get<std::vector<Contact>>();
    } catch (const nlohmann::json::exception& e) {
        in.close();
        std::cerr << "Error parsing " << path << ": " << e.what() << ". Renaming to .corrupt\n";
        std::string corruptPath = path + ".corrupt";
        std::remove(corruptPath.c_str());
        std::rename(path.c_str(), corruptPath.c_str());
        return {};
    }
}

bool saveContacts(const std::string& path, const std::vector<Contact>& list) {
    std::ofstream out(path);
    if (!out.is_open()) {
        return false;
    }
    nlohmann::json j = list;
    out << j.dump(2);
    return out.good();
}