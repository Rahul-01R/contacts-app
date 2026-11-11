#pragma once
#include <string>
#include <vector>

// Digits only; returns the last 10 digits, or "" if the phone is invalid.
std::string phoneKey(const std::string& phone);

// Removes spaces/tabs/newlines at both ends.
std::string trim(const std::string& s);

// Turns every run of spaces into a single space.
std::string collapseSpaces(const std::string& s);

// Lowercases A-Z only. Bytes 128 and above (Hindi, accents) are left alone.
std::string toLowerAscii(const std::string& s);

// Lowercases, then splits on every character that is not a letter or digit.
// Each distinct word appears once. "Mary-Anne O'Neil" -> mary, anne, o, neil
std::vector<std::string> splitWords(const std::string& s);

// trim + collapseSpaces. Keeps the user's capital letters.
std::string normalizeName(const std::string& name);

// Empty is fine. Otherwise: no spaces, exactly one @, text before it,
// a dot with text on both sides after it, at most 100 characters.
bool validEmail(const std::string& email);