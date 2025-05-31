#include "core/tick_loader.hpp"
#include "core/simulator.hpp"
#include "strategy/vwap_strategy.hpp"

#include <memory>
#include <filesystem>
#include <iostream>

int main() {
    std::string tick_path = "../data/aapl_0930_0935.csv";
    std::string out_dir = "../output/";

    std::cout << "Loading ticks from: " << std::filesystem::absolute(tick_path) << std::endl;
    std::vector<Tick> ticks = load_ticks(tick_path);
    
    if (ticks.empty()) {
        std::cerr << "no ticks loaded\n";
        return 1;
    }

    std::cout << "Successfully loaded " << ticks.size() << " ticks\n";

    auto strategy = std::make_unique<VWAPStrategy>(
        10000,                                      // target size
        "2025-05-28T09:30:00",                      // start time (UTC)
        "2025-05-28T09:35:00"                       // end time 
    );

    SimulatorEngine sim(std::move(strategy));
    sim.run(ticks);

    std::filesystem::create_directory(out_dir);
    sim.export_results(out_dir + "summary.json", out_dir + "trades.json");

    return 0;
}
