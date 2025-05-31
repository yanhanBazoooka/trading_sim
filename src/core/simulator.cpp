#include "core/simulator.hpp"
#include <iostream>

SimulatorEngine::SimulatorEngine(std::unique_ptr<StrategyEngine> strategy)
    : strategy_(std::move(strategy)) {}

void SimulatorEngine::run(const std::vector<Tick>& ticks) {
    std::cout << "Starting simulation with " << ticks.size() << " ticks\n";
    int orders_generated = 0;
    int orders_filled = 0;

    for (size_t i = 0; i < ticks.size(); ++i) {
        const Tick& tick = ticks[i];
        Order order = strategy_->generate_order(tick, i);

        if (order.size > 0) {
            orders_generated++;
            order.id = order_id_counter++;
            order.price = tick.price; // market execution for phase 1
            pnl_tracker_.record_fill(order);
            orders_filled++;
            
            std::cout << "Filled order: id=" << order.id
                      << " size=" << order.size
                      << " price=" << order.price
                      << " side=" << (order.side == OrderSide::BUY ? "BUY" : "SELL")
                      << "\n";
        }
    }
    
    std::cout << "Simulation complete:\n"
              << "  Orders generated: " << orders_generated << "\n"
              << "  Orders filled: " << orders_filled << "\n";
}

void SimulatorEngine::export_results(const std::string& summary_path, 
                                   const std::string& trades_path) const {
    std::cout << "Exporting results to:\n"
              << "  Summary: " << summary_path << "\n"
              << "  Trades: " << trades_path << "\n";
    pnl_tracker_.export_json(summary_path, trades_path);
}
