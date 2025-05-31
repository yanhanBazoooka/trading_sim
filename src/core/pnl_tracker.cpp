#include "core/pnl_tracker.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

void PnLTracker::record_fill(const Order& order) {
    if (order.size == 0) return;

    double trade_value = order.price * order.size;
    double trade_pnl = 0.0;

    if (order.side == OrderSide::BUY) {
        cash -= trade_value;
        avg_cost = (avg_cost * net_position + trade_value) / (net_position + order.size);
        net_position += order.size;
    } else {
        cash += trade_value;
        trade_pnl = (order.price - avg_cost) * order.size;
        realized_pnl += trade_pnl;
        net_position -= order.size;
        if (net_position == 0) avg_cost = 0.0; // reset if flat
    }

    trade_log.push_back(Trade{
        order.id,
        order.timestamp,
        order.price,
        order.size,
        order.side == OrderSide::BUY ? "BUY" : "SELL",
        trade_pnl
    });

    std::cout << "Recorded trade: id=" << order.id 
              << " pnl=" << trade_pnl
              << " net_pos=" << net_position 
              << " avg_cost=" << avg_cost << "\n";
}

void PnLTracker::export_json(const std::string& summary_path,
                             const std::string& trades_path) const {
    std::cout << "Creating trade log with " << trade_log.size() << " trades\n";
    
    json trades_json = json::array();
    for (const auto& t : trade_log) {
        trades_json.push_back({
            {"id", t.id},
            {"timestamp", t.timestamp},
            {"price", t.price},
            {"size", t.size},
            {"side", t.side},
            {"pnl", t.pnl}
        });
    }

    std::ofstream trades_out(trades_path);
    if (!trades_out.is_open()) {
        std::cerr << "Failed to open trades file: " << trades_path << "\n";
        return;
    }
    trades_out << trades_json.dump(2);
    trades_out.close();
    std::cout << "Wrote " << trade_log.size() << " trades to " << trades_path << "\n";

    json summary = {
        {"net_position", net_position},
        {"cash", cash},
        {"realized_pnl", realized_pnl},
        {"avg_cost", avg_cost}
    };

    std::ofstream summary_out(summary_path);
    if (!summary_out.is_open()) {
        std::cerr << "Failed to open summary file: " << summary_path << "\n";
        return;
    }
    summary_out << summary.dump(2);
    summary_out.close();
    std::cout << "Wrote summary to " << summary_path << "\n";
}
