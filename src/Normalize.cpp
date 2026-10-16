#include "Normalize.h"

std::string phoneKey(const std::string& phone) {
    std::string digits;
    for (char c : phone) {
        if (c >= '0' && c <= '9') {      // keep only digits
            digits.push_back(c);
        }
    }
    if (digits.size() < 10 || digits.size() > 15) {
        return "";                       // invalid
    }
    return digits.substr(digits.size() - 10);   // the last 10 digits
}