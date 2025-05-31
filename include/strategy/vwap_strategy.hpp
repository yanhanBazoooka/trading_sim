#pragma once
#include "strategy_engine.hpp"

class VWAPStrategy : public StrategyEngine {
public:
    Order generate_order(const Tick& tick, int tick_index) override;

private:
    double cumulative_price_volume = 0.0;
    int cumulative_volume = 0;
    double threshold_pct = 0.0005; // 0.05%
};
