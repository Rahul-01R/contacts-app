#include "Normalize.h"

#include <algorithm>
#include <cctype>

namespace {

bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Letters, digits, and any byte 128 or above count as part of a word.
bool isWordChar(char c) {
    unsigned char u = static_cast<unsigned char>(c);   // never pass a negative char
    return u >= 128 || std::isalnum(u) != 0;
}

}  // namespace

std::string phoneKey(const std::string& phone) {
    std::string digits;
    for (char c : phone) {
        if (c >= '0' && c <= '9') {
            digits.push_back(c);
        }
    }
    if (digits.size() < 10 || digits.size() > 15) {
        return "";
    }
    return digits.substr(digits.size() - 10);
}

std::string trim(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() && isSpace(s[start])) {
        start++;
    }
    std::size_t end = s.size();
    while (end > start && isSpace(s[end - 1])) {
        end--;
    }
    return s.substr(start, end - start);
}

std::string collapseSpaces(const std::string& s) {
    std::string out;
    bool previousWasSpace = false;
    for (char c : s) {
        if (c == ' ') {
            if (!previousWasSpace) {
                out.push_back(' ');
            }
            previousWasSpace = true;
        } else {
            out.push_back(c);
            previousWasSpace = false;
        }
    }
    return out;
}

std::string toLowerAscii(const std::string& s) {
    std::string out = s;
    for (char& c : out) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c + ('a' - 'A'));
        }
    }
    return out;
}

std::vector<std::string> splitWords(const std::string& s) {
    std::string lower = toLowerAscii(s);
    std::vector<std::string> words;
    std::string current;

    // Saves the word being built (if any) and starts a new one.
    auto finishWord = [&]() {
        if (current.empty()) {
            return;
        }
        if (std::find(words.begin(), words.end(), current) == words.end()) {
            words.push_back(current);       // keep each word only once
        }
        current.clear();
    };

    for (char c : lower) {
        if (isWordChar(c)) {
            current.push_back(c);
        } else {
            finishWord();
        }
    }
    finishWord();                            // the last word has no separator after it
    return words;
}

std::string normalizeName(const std::string& name) {
    return collapseSpaces(trim(name));
}

bool validEmail(const std::string& email) {
    if (email.empty()) {
        return true;
    }
    if (email.size() > 100) {
        return false;
    }
    if (email.find(' ') != std::string::npos) {
        return false;
    }

    std::size_t at = email.find('@');
    if (at == std::string::npos || at == 0) {
        return false;                        // no @, or nothing before it
    }
    if (email.find('@', at + 1) != std::string::npos) {
        return false;                        // a second @
    }

    std::string domain = email.substr(at + 1);
    for (std::size_t i = 1; i + 1 < domain.size(); i++) {
        if (domain[i] == '.') {
            return true;                     // a dot with text on both sides
        }
    }
    return false;
}