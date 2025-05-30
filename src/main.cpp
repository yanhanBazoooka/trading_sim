#include "tick_loader.hpp"
#include <iostream>

int main() {
    auto ticks = load_ticks("data/aapl_0930_0935.csv");
    std::cout << "loaded " << ticks.size() << " ticks\n";

    if (!ticks.empty()) {
        const auto& t = ticks[0];
        std::cout << "first tick: " << t.timestamp
                  << " $" << t.price
                  << " size: " << t.size
                  << (t.is_buy ? " [BUY]" : " [SELL]") << "\n";
    } else {
        std::cout << "no ticks loaded — check path and csv content\n";
    }
}
