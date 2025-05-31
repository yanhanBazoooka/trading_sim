#pragma once

#include "core/tick.hpp"
#include "core/order.hpp"
#include "core/pnl_tracker.hpp"
#include "strategy/strategy_engine.hpp"
#include <vector>
#include <memory>

class SimulatorEngine {
public:
    SimulatorEngine(std::unique_ptr<StrategyEngine> strategy);

    void run(const std::vector<Tick>& ticks);
    void write_outputs(const std::string& outdir = "output");

private:
    std::unique_ptr<StrategyEngine> strategy_;
    PnLTracker pnl_tracker_;
    int order_id_counter = 0;
};
