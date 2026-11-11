#include "Storage.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <cstdio> // Required for std::rename
using json = nlohmann::json;
std::vector<Contact> loadContacts(const std::string& filename) {
    std::vector<Contact> contacts;
    std::ifstream file(filename);
    
    // Gracefully handle missing file
    if (!file.is_open()) {
        return contacts; 
    }

    try {
        json j;
        file >> j;
        for (const auto& item : j) {
            contacts.push_back(item.get<Contact>());
        }
    } catch (const json::exception& e) {
        // Close file before attempting to rename it
        file.close();
        
        std::cerr << "[Storage Error] Corrupt JSON file detected: " << filename << "\n";
        std::cerr << "Exception: " << e.what() << "\n";
        
        // Rename corrupt file to save it for debugging, then return empty to start fresh
        std::string corruptName = filename + ".corrupt";
        if (std::rename(filename.c_str(), corruptName.c_str()) != 0) {
            std::cerr << "[Storage Error] Failed to rename corrupt file.\n";
        } else {
            std::cerr << "[Storage Error] File renamed to " << corruptName << ". Starting fresh.\n";
        }
    }

    return contacts;
}

bool saveContacts(const std::string& filename, const std::vector<Contact>& contacts) {
    std::ofstream file(filename);
    
    // Prevent server crash if folder is read-only
    if (!file.is_open()) {
        std::cerr << "[Storage Error] Cannot write to file: " << filename << ". Check folder permissions.\n";
        return false; // Returns false instead of empty return
    }

    json j = json::array();
    for (const auto& c : contacts) {
        j.push_back(c);
    }
    
    file << j.dump(4);
    return true; // Returns true on success
}