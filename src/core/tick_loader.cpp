#include "core/tick_loader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Tick> load_ticks(const std::string& filepath) {
    std::vector<Tick> ticks;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return ticks;
    }
    
    std::string line;
    std::getline(file, line); // skip header
    std::cout << "Header: " << line << std::endl;

    int count = 0;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string ts, price, size, side;
        std::getline(ss, ts, ',');
        std::getline(ss, price, ',');
        std::getline(ss, size, ',');
        std::getline(ss, side, ',');

        try {
            Tick tick{
                ts,
                std::stod(price),
                std::stoi(size),
                side == "B"
            };
            ticks.push_back(tick);
            count++;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing line: " << line << "\n";
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    std::cout << "Loaded " << count << " ticks from " << filepath << std::endl;
    return ticks;
}
