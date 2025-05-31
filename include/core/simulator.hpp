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

    // Process a single tick
    void process_tick(const Tick& tick);
    
    // Export results at any time
    void export_results(const std::string& summary_path, const std::string& trades_path) const;

private:
    std::unique_ptr<StrategyEngine> strategy_;
    PnLTracker pnl_tracker_;
    int order_id_counter = 0;
    int tick_counter = 0;
};
