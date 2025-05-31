#include "core/pnl_tracker.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

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
}

void PnLTracker::export_json(const std::string& summary_path,
                             const std::string& trades_path) const {
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
    trades_out << trades_json.dump(2);

    json summary = {
        {"net_position", net_position},
        {"cash", cash},
        {"realized_pnl", realized_pnl},
        {"avg_cost", avg_cost}
    };

    std::ofstream summary_out(summary_path);
    summary_out << summary.dump(2);
}
