#include "strategy/strategy_engine.hpp"

Order StrategyEngine::generate_order(const Tick& tick, int tick_index) {
    if (tick_index % 1000 != 0) {
        return Order{-1, "", OrderType::MARKET, OrderSide::BUY, 0, 0.0}; // null order
    }

    return Order{
        .id = tick_index,
        .timestamp = tick.timestamp,
        .type = OrderType::MARKET,
        .side = OrderSide::BUY,
        .size = 100,
        .price = tick.price
    };
}
