#pragma once
#include <string>

struct Tick {
    std::string timestamp;
    double price;
    int size;
    bool is_buy; // true = aggressive buy
};
