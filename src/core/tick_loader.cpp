#include "core/tick_loader.hpp"
#include <fstream>
#include <sstream>

std::vector<Tick> load_ticks(const std::string& filepath) {
    std::vector<Tick> ticks;
    std::ifstream file(filepath);
    std::string line;

    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string ts, price, size, side;
        std::getline(ss, ts, ',');
        std::getline(ss, price, ',');
        std::getline(ss, size, ',');
        std::getline(ss, side, ',');

        Tick tick{
            ts,
            std::stod(price),
            std::stoi(size),
            side == "B"
        };

        ticks.push_back(tick);
    }

    return ticks;
}
