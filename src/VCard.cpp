#include "VCard.h"
#include "Normalize.h" // For phoneKey()
#include <sstream>

// Helper to escape characters for vCard values
std::string escapeVCard(const std::string& val) {
    std::string out;
    for (char c : val) {
        if (c == '\\' || c == ',' || c == ';') {
            out += '\\';
            out += c;
        } else if (c == '\n') {
            out += "\\n";
        } else if (c != '\r') {
            out += c;
        }
    }
    return out;
}

std::string toVCard(const Contact& c) {
    std::ostringstream out;
    
    out << "BEGIN:VCARD\r\n";
    out << "VERSION:3.0\r\n";
    out << "FN:" << c.name << "\r\n";
    
    // Split name at the last space for the N line
    size_t lastSpace = c.name.find_last_of(' ');
    if (lastSpace == std::string::npos) {
        // One-word name goes in the given slot
        out << "N:;" << c.name << ";;;\r\n";
    } else {
        std::string given = c.name.substr(0, lastSpace);
        std::string family = c.name.substr(lastSpace + 1);
        out << "N:" << family << ";" << given << ";;;\r\n";
    }
    
    // Phone with +91 prefix using the normalized 10-digit key
    std::string pk = phoneKey(c.phone);
    if (!pk.empty()) {
        out << "TEL;TYPE=CELL:+91" << pk << "\r\n";
    }
    
    // Omit email and address if they are empty
    if (!c.email.empty()) {
        out << "EMAIL:" << escapeVCard(c.email) << "\r\n";
    }
    
    if (!c.address.empty()) {
        // Put the whole address in the third part out of seven
        out << "ADR;TYPE=HOME:;;" << escapeVCard(c.address) << ";;;;\r\n";
    }
    
    out << "END:VCARD\r\n";
    
    return out.str();
}