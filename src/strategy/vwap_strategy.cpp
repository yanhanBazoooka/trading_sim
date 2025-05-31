#include "strategy/vwap_strategy.hpp"

Order VWAPStrategy::generate_order(const Tick& tick, int tick_index) {
    // update internal VWAP state
    cumulative_price_volume += tick.price * tick.size;
    cumulative_volume += tick.size;

    if (cumulative_volume == 0) return Order{-1, "", OrderType::MARKET, OrderSide::BUY, 0, 0.0};

    double vwap = cumulative_price_volume / cumulative_volume;

    if (tick.price < vwap * (1.0 - threshold_pct)) {
        return Order{
            .id = tick_index,
            .timestamp = tick.timestamp,
            .type = OrderType::MARKET,
            .side = OrderSide::BUY,
            .size = 100,
            .price = tick.price
        };
    }

    return Order{-1, "", OrderType::MARKET, OrderSide::BUY, 0, 0.0}; // no-op
}
