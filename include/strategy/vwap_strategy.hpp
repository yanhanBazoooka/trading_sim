#pragma once
#include "strategy/strategy_engine.hpp"
#include <string>

class VWAPStrategy : public StrategyEngine {
public:
    VWAPStrategy(int target_qty,
                 const std::string& start_ts,
                 const std::string& end_ts);

    Order generate_order(const Tick& tick, int tick_index) override;

private:
    int target_quantity;
    int executed_quantity = 0;
    double cumulative_market_volume = 0.0;

    std::string start_time;
    std::string end_time;

    double allowed_slippage = 0.0015; // 0.15%
};
