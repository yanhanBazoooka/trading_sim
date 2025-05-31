#pragma once

#include "core/order.hpp"
#include <vector>
#include <string>

struct Trade {
    int id;
    std::string timestamp;
    double price;
    int size;
    std::string side;
    double pnl;  // new field for realized pnl per trade
};

class PnLTracker {
public:
    void record_fill(const Order& order);
    void export_json(const std::string& summary_path,
                     const std::string& trades_path) const;

private:
    std::vector<Trade> trade_log;
    int net_position = 0;
    double avg_cost = 0.0;
    double realized_pnl = 0.0;
    double cash = 0.0;
};
