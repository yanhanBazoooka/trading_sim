#include "core/simulator.hpp"
#include <iostream>

SimulatorEngine::SimulatorEngine(std::unique_ptr<StrategyEngine> strategy)
    : strategy_(std::move(strategy)) {}

void SimulatorEngine::run(const std::vector<Tick>& ticks) {
    for (size_t i = 0; i < ticks.size(); ++i) {
        const Tick& tick = ticks[i];

        Order order = strategy_->generate_order(tick, i);

        if (order.size > 0) {
            order.id = order_id_counter++;
            order.price = tick.price; // market execution for phase 1
            pnl_tracker_.record_fill(order);
        }
        // Debugging
        // std::cout << "[engine] submitting order id=" << order.id
        //   << " size=" << order.size
        //   << " price=" << order.price << "\n";
    }
}

void SimulatorEngine::write_outputs(const std::string& outdir) {
    pnl_tracker_.export_json(outdir + "/summary.json", outdir + "/trades.json");
}
