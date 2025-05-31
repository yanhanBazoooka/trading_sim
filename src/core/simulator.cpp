#include "core/simulator.hpp"
#include <iostream>

SimulatorEngine::SimulatorEngine(std::unique_ptr<StrategyEngine> strategy)
    : strategy_(std::move(strategy)) {}

void SimulatorEngine::process_tick(const Tick& tick) {
    Order order = strategy_->generate_order(tick, tick_counter++);

    if (order.size > 0) {
        order.id = order_id_counter++;
        order.price = tick.price; // market execution for phase 1
        pnl_tracker_.record_fill(order);
        
        std::cout << "Filled order: id=" << order.id
                  << " size=" << order.size
                  << " price=" << order.price
                  << " side=" << (order.side == OrderSide::BUY ? "BUY" : "SELL")
                  << "\n";
    }
}

void SimulatorEngine::export_results(const std::string& summary_path, 
                                   const std::string& trades_path) const {
    std::cout << "Exporting results to:\n"
              << "  Summary: " << summary_path << "\n"
              << "  Trades: " << trades_path << "\n";
    pnl_tracker_.export_json(summary_path, trades_path);
}
