#include "strategy/vwap_strategy.hpp"
#include "core/tick_loader.hpp"

#include <algorithm>
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace std::chrono;

VWAPStrategy::VWAPStrategy(int target_qty,
                           const std::string& start_ts,
                           const std::string& end_ts)
    : target_quantity(target_qty),
      start_time(start_ts),
      end_time(end_ts) {
    std::cout << "VWAP Strategy initialized with:\n"
              << "  Target Qty: " << target_qty << "\n"
              << "  Start Time: " << start_ts << "\n"
              << "  End Time: " << end_ts << "\n";
}

static system_clock::time_point parse_time(const std::string& ts) {
    std::istringstream ss(ts);
    std::tm t = {};
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
    return system_clock::from_time_t(std::mktime(&t));
}

Order VWAPStrategy::generate_order(const Tick& tick, int tick_index) {
    system_clock::time_point t0 = parse_time(start_time);
    system_clock::time_point t1 = parse_time(end_time);
    system_clock::time_point now = parse_time(tick.timestamp.substr(0, 19)); // trim millis

    if (now < t0 || now > t1) {
        return Order{-1, "", OrderType::MARKET, OrderSide::BUY, 0, 0.0};
    }

    auto elapsed = duration_cast<seconds>(now - t0).count();
    auto total = duration_cast<seconds>(t1 - t0).count();
    double time_progress = std::clamp(elapsed / static_cast<double>(total), 0.0, 1.0);

    cumulative_market_volume += tick.size;
    double volume_progress = cumulative_market_volume > 0
        ? executed_quantity / cumulative_market_volume
        : 0.0;

    // If we're significantly behind schedule, BUY
    if (volume_progress < time_progress - allowed_slippage) {
        int remaining = target_quantity - executed_quantity;
        if (remaining <= 0) return Order{-1, "", OrderType::MARKET, OrderSide::BUY, 0, 0.0};

        int clip = std::min(100, remaining);
        executed_quantity += clip;

        std::cout << "BUY order at tick " << tick_index 
                  << " (time progress: " << std::fixed << std::setprecision(3) << time_progress
                  << ", volume progress: " << volume_progress 
                  << ", clip size: " << clip << ")\n";

        return Order{
            .id = tick_index,
            .timestamp = tick.timestamp,
            .type = OrderType::MARKET,
            .side = OrderSide::BUY,
            .size = clip,
            .price = tick.price
        };
    }
    // If we're significantly ahead of schedule, SELL
    else if (volume_progress > time_progress + allowed_slippage) {
        int excess = executed_quantity - static_cast<int>(time_progress * target_quantity);
        if (excess <= 0) return Order{-1, "", OrderType::MARKET, OrderSide::BUY, 0, 0.0};

        int clip = std::min(100, excess);
        executed_quantity -= clip;

        std::cout << "SELL order at tick " << tick_index 
                  << " (time progress: " << std::fixed << std::setprecision(3) << time_progress
                  << ", volume progress: " << volume_progress 
                  << ", clip size: " << clip << ")\n";

        return Order{
            .id = tick_index,
            .timestamp = tick.timestamp,
            .type = OrderType::MARKET,
            .side = OrderSide::SELL,
            .size = clip,
            .price = tick.price
        };
    }

    return Order{-1, "", OrderType::MARKET, OrderSide::BUY, 0, 0.0};
}
