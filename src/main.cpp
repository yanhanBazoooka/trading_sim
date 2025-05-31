#include "core/tick_loader.hpp"
#include "strategy/vwap_strategy.hpp"
#include <iostream>

int main() {
    auto tick_data_path = "../data/aapl_0930_0935.csv";
    auto ticks = load_ticks(tick_data_path);
    std::cout << "loaded " << ticks.size() << " ticks\n";

    VWAPStrategy strategy;

    for (size_t i = 0; i < ticks.size(); ++i) {
        auto order = strategy.generate_order(ticks[i], i);
        if (order.size > 0) {
            std::cout << "order: " << order.timestamp << " $" << order.price
                      << " size " << order.size << "\n";
        }
    }

    return 0;
}
