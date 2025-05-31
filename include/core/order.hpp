#pragma once
#include <string>

enum class OrderType {
    MARKET,
    LIMIT
};

enum class OrderSide {
    BUY,
    SELL
};

struct Order {
    int id;                     // unique order id
    std::string timestamp;      // order entry time
    OrderType type;
    OrderSide side;
    int size;
    double price;               // limit price (or market snapshot price)
};
