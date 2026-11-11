#pragma once
#include <string>

struct Contact {
    int id = 0;
    std::string name;
    std::string phone;   // as the user typed it
    std::string email;
    std::string address;
};