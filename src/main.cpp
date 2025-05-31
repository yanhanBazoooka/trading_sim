#include "core/tick_loader.hpp"
#include "core/simulator.hpp"
#include "strategy/vwap_strategy.hpp"

#include <iostream>
#include <memory>
#include <fstream>


int main() {
    auto ticks = load_ticks("../data/aapl_0930_0935.csv");
    std::cout << "loaded " << ticks.size() << " ticks\n";

    if (ticks.empty()) {
        std::cerr << "no tick data loaded, aborting.\n";
        return 1;
    }

    auto strategy = std::make_unique<VWAPStrategy>();
    SimulatorEngine engine(std::move(strategy));

    engine.run(ticks);
    engine.write_outputs("../output");

    std::cout << "✅ simulation complete. results written to /output\n";
    return 0;
}
